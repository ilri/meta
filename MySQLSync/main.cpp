#include <stdlib.h>
#include <QtGui/QApplication>
#include <tclap/CmdLine.h>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QVariant>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QStringList>
#include <QSqlField>
#include <QList>
#include <QDebug>
#include <QByteArray>

struct stmtdef
{
  long stmtID;
  QString stmtSQL;
};
typedef stmtdef TstmtDef;

QList<TstmtDef> stmts;

struct insdatadef
{
  QString name;
  QString value;
};
typedef insdatadef Tinsdatadef;

QList<Tinsdatadef> insdata;

bool lkpComp(TstmtDef left, TstmtDef right)
{
  return left.stmtID < right.stmtID;
}

int getFieldIndex(QSqlQuery query,QString fieldName)
{
    return query.record().indexOf(fieldName); //Return the index of fieldName;
}

void log(QString message)
{
    QString temp;
    temp = message + "\n";
    printf(temp.toLocal8Bit().data());
}

void getHexDAta(QString data)
{
    QStringList temp;
    temp = data.split(",", QString::SkipEmptyParts);
    QString field;
    QString hexdata;
    QString sdata;
    QByteArray ba;
    int pos;
    int pos2;
    for (pos = 0; pos <= temp.count()-1;pos++)
    {
        pos2 = temp[pos].indexOf(")");
        field = temp[pos].left(pos2+1);
        hexdata = temp[pos].right(temp[pos].length()-(pos2+1));
        field = field.replace("(","");
        field = field.replace(")","");
        ba = QByteArray::fromHex(hexdata.toAscii());
        sdata = QString::fromAscii(ba.data());
        Tinsdatadef datadef;
        datadef.name = field;
        datadef.value = sdata;
        insdata.append(datadef);
    }
}

void getKeyDAta(QString data)
{
    QStringList temp;
    temp = data.split(",", QString::SkipEmptyParts);
    QString key;
    QString sdata;
    int pos;
    int pos2;
    for (pos = 0; pos <= temp.count()-1;pos++)
    {
        pos2 = temp[pos].indexOf(")");
        key = temp[pos].left(pos2+1);
        sdata = temp[pos].right(temp[pos].length()-(pos2+1));
        key = key.replace("(","");
        key = key.replace(")","");
        Tinsdatadef datadef;
        datadef.name = key;
        datadef.value = sdata;
        insdata.append(datadef);
    }
}

int synchronise(QSqlDatabase db,QSqlDatabase dblite, QString outFile, QString logFile)
{
    QFile file(outFile);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        log("Cannot create output statement file");
        return 1;
    }

    QFile logfile(logFile);
    if (!logfile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        log("Cannot create log file");
        return 1;
    }

    QTextStream out(&file);
    QTextStream logout(&logfile);

    QSqlQuery audit(dblite);    
    QString sql;


    QStringList updateKeys;
    QString keyField;
    QString keyValue;

    int tmp;
    int auditNumber;
    int cols;

    auditNumber = 0;

    sql = "SELECT audit_id,audit_date,audit_action,audit_user,audit_table,audit_column,audit_key";
    sql = sql + " FROM audit_log WHERE audit_action = 'DELETE' AND audit_table <> 'audit_log'";
    sql = sql + " ORDER BY audit_id";

    if (audit.exec(sql))
    {
        sql = "";
        while (audit.next())
        {

            updateKeys.clear();
            auditNumber = audit.value(0).toInt();
            sql = "DELETE FROM " + audit.value(4).toString() + " WHERE ";
            updateKeys.append(audit.value(6).toString().split(",",QString::SkipEmptyParts));
            for (int pos = 0; pos <= updateKeys.count() -1 ; pos++)
            {
                tmp = updateKeys[pos].indexOf(")");
                keyField = updateKeys[pos].left(tmp+1);
                keyField = keyField.remove("(");
                keyField = keyField.remove(")");
                keyValue = updateKeys[pos].right(updateKeys[pos].length()-(tmp+1));
                sql = sql + keyField + " = '" + keyValue + "' AND ";
            }
            sql = sql.left(sql.length()-5);
            TstmtDef dstm;
            dstm.stmtID = auditNumber;
            dstm.stmtSQL = sql;
            stmts.append(dstm);

        }
    }
    else
    {
        log("Cannot query the audit log for deletes. The file might not be an snapshot");
        log(dblite.lastError().databaseText());
        return 1;
    }

    sql = "SELECT audit_id,audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,";
    sql = sql + "audit_oldvalue,audit_newvalue FROM audit_log WHERE audit_action = 'UPDATE' AND audit_table <> 'audit_log'";
    sql = sql + " ORDER BY audit_id";

    if (audit.exec(sql))
    {
        sql = "";
        while (audit.next())
        {
            if (audit.value(7).toString() != audit.value(8).toString())
            {
                updateKeys.clear();
                auditNumber = audit.value(0).toInt();
                sql = "UPDATE " + audit.value(4).toString() + " SET " + audit.value(5).toString();

                if (!audit.value(8).isNull())
                    sql = sql + " = '" + audit.value(8).toString() + "'";
                else
                    sql = sql + " = NULL";

                sql = sql + " WHERE ";
                updateKeys.append(audit.value(6).toString().split(",",QString::SkipEmptyParts));
                for (int pos = 0; pos <= updateKeys.count() -1 ; pos++)
                {
                    tmp = updateKeys[pos].indexOf(")");
                    keyField = updateKeys[pos].left(tmp+1);
                    keyField = keyField.remove("(");
                    keyField = keyField.remove(")");
                    keyValue = updateKeys[pos].right(updateKeys[pos].length()-(tmp+1));
                    sql = sql + keyField + " = '" + keyValue + "' AND ";
                }
                sql = sql.left(sql.length()-5);
                TstmtDef ustm;
                ustm.stmtID = auditNumber;
                ustm.stmtSQL = sql;
                stmts.append(ustm);
            }
        }
    }
    else
    {
        log("Cannot query the audit log for updates. The file might not be an snapshot");
        log(dblite.lastError().databaseText());
        return 1;
    }

    sql = "SELECT audit_id,audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,";
    sql = sql + "audit_insdeldata FROM audit_log WHERE audit_action = 'INSERT'";
    sql = sql + " ORDER BY audit_id";
    if (audit.exec(sql))
    {
        while (audit.next())
        {
            auditNumber = audit.value(0).toInt();
            sql = "INSERT INTO " + audit.value(4).toString() + "(";
            insdata.clear();
            getKeyDAta(audit.value(6).toString());
            getHexDAta(audit.value(7).toString());
            for (cols = 0; cols <= insdata.count()-1; cols++)
            {
                sql = sql + insdata[cols].name + ",";
            }
            sql = sql.left(sql.length()-1);
            sql = sql + ") VALUES (";
            for (cols = 0; cols <= insdata.count()-1; cols++)
            {
                sql = sql + "'" + insdata[cols].value + "',";
            }
            sql = sql.left(sql.length()-1);
            sql = sql + ")";

            sql = sql.replace("''","NULL"); //Convert all empty characters to NULL... safer

            TstmtDef istm;
            istm.stmtID = auditNumber;
            istm.stmtSQL = sql;
            stmts.append(istm);
        }
    }
    else
    {
        log("Cannot query the audit log for inserts. The file might not be an snapshot");
        log(dblite.lastError().databaseText());
        return 1;
    }

    QSqlQuery execMyDB(db);
    bool errors;
    errors = false;
    log("Synchronising " + QString::number(stmts.count()) + " changes");

    int total;
    int pos;
    int perc;
    pos = 0;
    total = stmts.count();

    for (tmp = 0; tmp <= stmts.count()-1; tmp++ )
    {
        stmts[tmp].stmtSQL.replace("'None'","NULL");
    }

    qSort(stmts.begin(),stmts.end(),lkpComp);
    for (tmp = 0; tmp <= stmts.count()-1; tmp++ )
    {
        out << stmts[tmp].stmtID << ":" << stmts[tmp].stmtSQL << "\n";
        pos++;
        perc = (pos *100) / total;
        if (tmp < stmts.count()-1)
            printf("\r%i %% executed", perc);
        else
            printf("\r%i %% executed", 100);
        fflush(stdout);

        if (execMyDB.exec(stmts[tmp].stmtSQL))
        {
            if (execMyDB.numRowsAffected() > 1)
            {
                logout << "Warning: Statement " + QString::number(stmts[tmp].stmtID) + " affected more than one row. Statement: " + stmts[tmp].stmtSQL << "\n";
                errors = true;
            }
            if (execMyDB.numRowsAffected() == 0)
            {
                logout << "Warning: Statement " + QString::number(stmts[tmp].stmtID) + " did not affect any rows. Statement: " + stmts[tmp].stmtSQL << "\n";
                errors = true;
            }
        }
        else
        {
            logout << "Error in statement " + QString::number(stmts[tmp].stmtID) + ". Error: " + execMyDB.lastError().databaseText() + ". Statement: " + stmts[tmp].stmtSQL << "\n";
            errors = true;
        }
    }
    if (errors)
    {
        log("Errors/warnings were encountered in the process. Check the log file " + logFile);
    }

    file.close();
    return 0;
}

int main(int argc, char *argv[])
{
    QString title;
    title = title + "****************************************************************** \n";
    title = title + " * MySQLSynchrorise 1.0                                           * \n";
    title = title + " * This tool synchronise a MySQL database againts a SQite         * \n";
    title = title + " * snapshot file.                                                 * \n";
    title = title + " * This tool is part of CSPro Tools (c) ILRI, 2013                * \n";
    title = title + " ****************************************************************** \n";

    TCLAP::CmdLine cmd(title.toAscii().data(), ' ', "1.0 (Beta 1)");
    //Required arguments
    TCLAP::ValueArg<std::string> hostArg("H","host","MySQL host. Default localhost",false,"localhost","string");
    TCLAP::ValueArg<std::string> portArg("P","port","MySQL port. Default 3306.",false,"3306","string");
    TCLAP::ValueArg<std::string> userArg("u","user","User to connect to MySQL",true,"","string");
    TCLAP::ValueArg<std::string> passArg("p","password","Password to connect to MySQL",true,"","string");
    TCLAP::ValueArg<std::string> schemaArg("s","schema","Schema in MySQL",true,"","string");
    TCLAP::ValueArg<std::string> sqliteArg("S","sqlite","The sqlite snapshot file",true,"","string");
    TCLAP::ValueArg<std::string> outArg("o","output","Output statement file",false,"./output.sql","string");
    TCLAP::ValueArg<std::string> logArg("l","log","Output log file",false,"./output.log","string");


    cmd.add(hostArg);
    cmd.add(portArg);
    cmd.add(userArg);
    cmd.add(passArg);
    cmd.add(schemaArg);
    cmd.add(sqliteArg);
    cmd.add(outArg);
    cmd.add(logArg);
    //Parsing the command lines
    cmd.parse( argc, argv );

    //Getting the variables from the command

    QString host = QString::fromUtf8(hostArg.getValue().c_str());
    QString port = QString::fromUtf8(portArg.getValue().c_str());
    QString user = QString::fromUtf8(userArg.getValue().c_str());
    QString pass = QString::fromUtf8(passArg.getValue().c_str());
    QString schema = QString::fromUtf8(schemaArg.getValue().c_str());
    QString sqlite = QString::fromUtf8(sqliteArg.getValue().c_str());
    QString output = QString::fromUtf8(outArg.getValue().c_str());
    QString outputlog = QString::fromUtf8(logArg.getValue().c_str());

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
            if (QFile::exists(sqlite))
            {
                dblite.setDatabaseName(sqlite);
                if (dblite.open())
                {
                    if (synchronise(db,dblite,output,outputlog) > 0)
                    {
                        db.close();
                        dblite.close();
                        return 1;
                    }
                }
                else
                {
                    db.close();
                    log("Cannot connect to the snapshot database");
                    log(dblite.lastError().databaseText());
                    return 1;
                }
            }
            else
            {
                db.close();
                log("The snapshot file does not exists");
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
    qDebug("Done");
    return 0;
}
