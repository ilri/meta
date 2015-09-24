#include <tclap/CmdLine.h>
#include <QSqlDatabase>
#include <stdlib.h>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QVariant>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QSqlField>
#include <QStringList>
#include <QProcess>
#include <QByteArray>
#include <QThread>
#include "sqlimport.h"

void log(QString message)
{
    QString temp;
    temp = message + "\n";
    printf(temp.toLocal8Bit().data());
}

int genSnapHost(QSqlDatabase db, QSqlDatabase dblite, QString auditFile, QString host, QString user, QString pass, QString schema)
{
    QSqlQuery tables(db);
    QString sql;
    QStringList ignoreTables;
    int pos;

    ignoreTables << "dict_grpinfo";
    ignoreTables << "dict_tblinfo";
    ignoreTables << "dict_clminfo";
    ignoreTables << "dict_relinfo";
    ignoreTables << "dict_childinfo";
    ignoreTables << "audit_log";
    ignoreTables << "dict_dctiso639";
    ignoreTables << "dict_iso639";
    ignoreTables << "dict_lkpiso639";




    //Authorisation tables
    ignoreTables << "auth_group";
    ignoreTables << "auth_group_permissions";
    ignoreTables << "auth_permission";
    ignoreTables << "auth_user";
    ignoreTables << "auth_user_groups";
    ignoreTables << "auth_user_user_permissions";
    ignoreTables << "user_log";


    //Django tables
    ignoreTables << "auth_message";
    ignoreTables << "django_admin_log";
    ignoreTables << "django_content_type";
    ignoreTables << "django_session";
    ignoreTables << "django_site";

    //Retrives any views in the schema and add them to the list of ignored tables
    sql = "show full tables where Table_Type = 'VIEW'";
    if (tables.exec(sql))
    {
        while (tables.next())
        {
            ignoreTables << tables.value(0).toString();
        }
    }
    else
    {
        log("Error reading the tables. MySQL Error:" + tables.lastError().databaseText());
        return 1;
    }

    QString program = "mysqldump";
    QStringList arguments;
    arguments << "--single-transaction";
    arguments << "-h" << host;
    arguments << "-u" << user;
    arguments << "--password=" + pass;
    arguments << "--skip-triggers";
    for (pos = 0; pos <= ignoreTables.count()-1; pos++)
        arguments << "--ignore-table=" + schema + "." + ignoreTables[pos];
    arguments << schema;


    QProcess *myProcess = new QProcess();
    log("Running MySQLDump please wait....");
    if (QFile::exists("./tempDump.sql"))
        if (!QFile::remove("./tempDump.sql"))
        {
            log("Error removing temporary dump file");
            return 1;
        }

    if (QFile::exists("./tempDump_lite.sql"))
        if (!QFile::remove("./tempDump_lite.sql"))
        {
            log("Error removing temporary sqlite file");
            return 1;
        }

    myProcess->setStandardOutputFile("./tempDump.sql");
    myProcess->start(program, arguments);
    myProcess->waitForFinished(-1);
    if ((myProcess->exitCode() > 0) || (myProcess->error() == QProcess::FailedToStart))
    {
        if (myProcess->error() == QProcess::FailedToStart)
        {
            log("Error: Command " +  program + " not found");
        }
        else
        {
            log("Running MySQLDump returned error");
            QString serror = myProcess->readAllStandardError();
            log(serror);
            log("Running paremeters:" + arguments.join(" "));
        }
        return 1;
    }
    log("Dump file created. Corverting it to sqlite syntax");

    sqlimport *importproc = new sqlimport;
    importproc->inputFile = "./tempDump.sql";
    importproc->outputFile = "./tempDump_lite.sql";
    importproc->processInserts = true;
    importproc->translateConstraints = true;
    importproc->start();
    importproc->wait();

    log("Dump file converted. Creating snapshot");

    QFile file("./tempDump_lite.sql");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        log("Error openning temporary sqlite file");
        return 1;
    }

    QSqlQuery creates(dblite);
    while (!file.atEnd())
    {
        QByteArray line = file.readLine();

        if (!line.simplified().isEmpty())
        {
            if (line.left(2) != "--")
            {
                if (!creates.exec(line))
                {
                    log("Error executing statement");
                    log(creates.lastError().databaseText());
                    creates.exec("ROLLBACK;");
                    file.close();
                    return 1;
                }
            }
        }
    }

    file.close();

    log("Appending audit to snapshot");

    program = "sqlite3";
    arguments.clear();
    arguments << "-bail";
    arguments << dblite.databaseName();

    myProcess->setStandardInputFile(auditFile);
    myProcess->start(program, arguments);
    myProcess->waitForFinished(-1);
    if ((myProcess->exitCode() > 0) || (myProcess->error() == QProcess::FailedToStart))
    {
        if (myProcess->error() == QProcess::FailedToStart)
        {
            log("Error: Command " +  program + " not found");
        }
        else
        {
            log("Running Sqlite3 returned error");
            QString serror = myProcess->readAllStandardError();
            log(serror);
            log("Running paremeters:" + arguments.join(" "));
        }
        return 1;
    }

    log("Snapshot created successfully");

    return 0;
}

int main(int argc, char *argv[])
{
    QString title;
    title = title + "****************************************************************** \n";
    title = title + " * GenSnapShot 1.0                                                * \n";
    title = title + " * This tool generates a snapshot file from a MySQL schema.       * \n";
    title = title + " * The tool relies on MySQLDump, sqlite3 and the dictionary       * \n";
    title = title + " * tables to convert a MySQL dump file into a sqlite database.    * \n";
    title = title + " * Both MySQLDump and sqlite3 must be in path.                    * \n";
    title = title + " * This tool is part of META (c) ILRI, 2013                       * \n";
    title = title + " ****************************************************************** \n";

    TCLAP::CmdLine cmd(title.toLatin1().constData(), ' ', "1.0 (Beta 1)");
    //Required arguments
    TCLAP::ValueArg<std::string> hostArg("H","host","MySQL Host. Default localhost",false,"localhost","string");
    TCLAP::ValueArg<std::string> portArg("P","port","MySQL Port. Default 3306",false,"3306","string");
    TCLAP::ValueArg<std::string> userArg("u","user","MySQL user",true,"","string");
    TCLAP::ValueArg<std::string> passArg("p","password","MySQL password",true,"","string");
    TCLAP::ValueArg<std::string> schemaArg("s","schema","MySQL schema",true,"","string");
    TCLAP::ValueArg<std::string> auditArg("a","audit","Input audit file",true,"","string");
    TCLAP::ValueArg<std::string> outputArg("o","output","Ooutput snapshot file",true,"","string");


    cmd.add(hostArg);
    cmd.add(portArg);
    cmd.add(userArg);
    cmd.add(passArg);
    cmd.add(schemaArg);
    cmd.add(auditArg);
    cmd.add(outputArg);
    //Parsing the command lines
    cmd.parse( argc, argv );

    //Getting the variables from the command


    QString host = QString::fromUtf8(hostArg.getValue().c_str());
    QString port = QString::fromUtf8(portArg.getValue().c_str());
    QString user = QString::fromUtf8(userArg.getValue().c_str());
    QString pass = QString::fromUtf8(passArg.getValue().c_str());
    QString schema = QString::fromUtf8(schemaArg.getValue().c_str());
    QString auditFile = QString::fromUtf8(auditArg.getValue().c_str());
    QString outputFile = QString::fromUtf8(outputArg.getValue().c_str());

    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL","MyDB");
        QSqlDatabase dblite = QSqlDatabase::addDatabase("QSQLITE","DBLite");
        db.setHostName(host);
        db.setPort(port.toInt());
        db.setDatabaseName(schema);
        db.setUserName(user);
        db.setPassword(pass);
        if (db.open())
        {
            if (!QFile::exists(outputFile))
            {
                dblite.setDatabaseName(outputFile);
                if (dblite.open())
                {
                    if (genSnapHost(db,dblite,auditFile,host,user,pass,schema) > 0)
                    {
                        db.close();
                        dblite.close();
                        return 1;
                    }
                }
                else
                {
                    db.close();
                    log("Cannot create snapshot file");
                    log(dblite.lastError().databaseText());
                    return 1;
                }
            }
            else
            {
                db.close();
                log("The snapshot file already exists");
                return 1;
            }
        }
        else
        {
            log("Cannot connect to database");
            log(db.lastError().databaseText());
            return 1;
        }
    }

    return 0;
}
