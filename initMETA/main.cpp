#include <QCoreApplication>
#include <stdlib.h>
#include <tclap/CmdLine.h>
#include <QStringList>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QVariant>
#include <QTextStream>
#include <QFile>
#include <QDir>

QStringList tables;
QSqlQuery references;
QStringList ignoreTables;

void log(QString message, bool newLine = true)
{
    QString temp;
    if (newLine)
        temp = message + "\n";
    else
        temp = message;
    printf(temp.toLocal8Bit().data());
}

int getFieldIndex(QSqlQuery query,QString fieldName)
{
    return query.record().indexOf(fieldName); //Return the index of fieldName;
}

//Get the minimum index of parent of a table
int getMaxParentIndex(QString table)
{
    int res;
    res = -1;
    references.first();
    while (references.isValid())
    {
        if (references.value(0).toString() == table)
        {
            if (tables.indexOf(references.value(1).toString()) >= res)
                res = tables.indexOf(references.value(1).toString());
        }
        references.next();
    }
    return res;
}

bool isIgnoreTable(QString table)
{
    bool res;
    res = false;
    for (int pos = 0; pos <= ignoreTables.count()-1;pos++)
    {
        if (ignoreTables[pos].toUpper() == table.toUpper())
        {
            res = true;
            break;
        }
    }
    return res;
}

int createMETAAudit(QSqlDatabase mydb, QString auditDir)
{
    QSqlQuery query(mydb);
    QSqlQuery query2(mydb);
    QString sql;
    QStringList TriggerData;
    QStringList TriggerLite;

    QStringList dropMyTriggers;
    QStringList dropLiteTriggers;

    TriggerData << "CREATE TABLE IF NOT EXISTS audit_log (";
    TriggerData << "audit_id INT(11) NOT NULL AUTO_INCREMENT ,";
    TriggerData << "audit_date TIMESTAMP NULL ,";
    TriggerData << "audit_action VARCHAR(6) NULL ,";
    TriggerData << "audit_user VARCHAR(120) NULL ,";
    TriggerData << "audit_table VARCHAR(60) NULL ,";
    TriggerData << "audit_column VARCHAR(60) NULL ,";
    TriggerData << "audit_key VARCHAR(500) NULL ,";
    TriggerData << "audit_oldvalue VARCHAR(500) NULL ,";
    TriggerData << "audit_newvalue VARCHAR(500) NULL ,";
    TriggerData << "audit_insdeldata TEXT NULL ,";
    TriggerData << "PRIMARY KEY (audit_id) )";
    TriggerData << " ENGINE = InnoDB CHARSET=utf8;";
    TriggerData << "";

    //TriggerData << "ALTER TABLE audit_log ADD COLUMN audit_insdeldata TEXT NULL;";
    //TriggerData << "ALTER TABLE audit_log CHANGE audit_updatekey audit_key varchar(500);";
    //TriggerData << "";


    TriggerLite << "BEGIN;";
    TriggerLite << "CREATE TABLE audit_log (";
    TriggerLite << "audit_id INTEGER NOT NULL,";
    TriggerLite << "audit_date VARCHAR(120) NULL ,";
    TriggerLite << "audit_action VARCHAR(6) NULL ,";
    TriggerLite << "audit_user VARCHAR(120) NULL ,";
    TriggerLite << "audit_table VARCHAR(60) NULL ,";
    TriggerLite << "audit_column VARCHAR(60) NULL ,";
    TriggerLite << "audit_key VARCHAR(500) NULL ,";
    TriggerLite << "audit_oldvalue VARCHAR(500) NULL ,";
    TriggerLite << "audit_newvalue VARCHAR(500) NULL ,";
    TriggerLite << "audit_insdeldata TEXT NULL ,";
    TriggerLite << "PRIMARY KEY (audit_id) );";
    TriggerLite << "";



    int auditIndex;
    auditIndex = 0;

    sql = "SELECT tbl_cod FROM dict_tblinfo WHERE tbl_cod NOT LIKE 'v_%'"; //Excluse views in audit
    if (query.exec(sql))
    {
        while (query.next())
        {
            auditIndex = auditIndex + 1;
            if (ignoreTables.indexOf(query.value(0).toString().toLower()) < 0)
            {

                //Update trigger for MySQL-------------------------------------------------------------------

                dropMyTriggers << "DROP TRIGGER audit_" + QString::number(auditIndex) +"_update;";

                TriggerData << "delimiter $$";
                TriggerData << "CREATE TRIGGER audit_" + QString::number(auditIndex) +"_update";
                TriggerData << "AFTER UPDATE ON " + query.value(0).toString();
                TriggerData << "FOR EACH ROW BEGIN";
                TriggerData << "DECLARE ts TIMESTAMP DEFAULT CURRENT_TIMESTAMP();";
                TriggerData << "DECLARE us VARCHAR(120) DEFAULT USER();";
                TriggerData << "";

                QString mykeyData;
                mykeyData = "CONCAT(";

                QString myNoKeyData;

                sql = "SELECT clm_cod FROM dict_clminfo WHERE tbl_cod = '"  + query.value(0).toString() + "' AND clm_key = 1";
                if (query2.exec(sql))
                {
                    while (query2.next())
                    {
                        mykeyData = mykeyData + "'(" + query2.value(0).toString() + ")',OLD." + query2.value(0).toString() + ",',',";
                    }
                    mykeyData = mykeyData.left(mykeyData.length()-5) + ")";
                }

                sql = "SELECT clm_cod FROM dict_clminfo WHERE tbl_cod = '"  + query.value(0).toString() + "' ORDER BY clm_key,clm_pos DESC"; // AND clm_key = 0
                if (query2.exec(sql))
                {
                    while (query2.next())
                    {
                        TriggerData << "IF OLD." + query2.value(0).toString() + " <> NEW." + query2.value(0).toString() + " THEN INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) VALUES (ts,'UPDATE',us,'" + query.value(0).toString() + "','" + query2.value(0).toString() + "'," + mykeyData + ",OLD." + query2.value(0).toString() + ",NEW." + query2.value(0).toString() + ");";
                        TriggerData << "END IF;";
                    }
                }

                TriggerData << "";
                TriggerData << "END$$";
                TriggerData << "DELIMITER ;";
                TriggerData << "";

                //Insert trigger for MySQL-----------------------------------------------------------------------------------

                dropMyTriggers << "DROP TRIGGER audit_" + QString::number(auditIndex) +"_insert;";

                TriggerData << "delimiter $$";
                TriggerData << "CREATE TRIGGER audit_" + QString::number(auditIndex) +"_insert";
                TriggerData << "AFTER INSERT ON " + query.value(0).toString();
                TriggerData << "FOR EACH ROW BEGIN";
                TriggerData << "DECLARE ts TIMESTAMP DEFAULT CURRENT_TIMESTAMP();";
                TriggerData << "DECLARE us VARCHAR(120) DEFAULT USER();";
                TriggerData << "";

                //Getting the key data of the deleted record
                sql = "SELECT clm_cod FROM dict_clminfo WHERE tbl_cod = '"  + query.value(0).toString() + "' AND clm_key = 1";
                if (query2.exec(sql))
                {
                    mykeyData = "CONCAT(";
                    while (query2.next())
                    {
                        mykeyData = mykeyData + "'(" + query2.value(0).toString() + ")',NEW." + query2.value(0).toString() + ",',',";
                    }
                }
                mykeyData = mykeyData.left(mykeyData.length()-5) + ")";

                // Getting the non-key data of the deleted record
                // The non-key data is represented as Hexadecimal character values to avoid conflicts with controlling characters like () and ,
                sql = "SELECT clm_cod FROM dict_clminfo WHERE tbl_cod = '"  + query.value(0).toString() + "' AND clm_key = 0";
                if (query2.exec(sql))
                {
                    myNoKeyData = "CONCAT(";
                    while (query2.next())
                    {
                        myNoKeyData = myNoKeyData + "'(" + query2.value(0).toString() + ")',ifnull(hex(cast(NEW." + query2.value(0).toString() + " as char)),''),',',";
                    }
                }
                if (myNoKeyData != "CONCAT(")
                    myNoKeyData = myNoKeyData.left(myNoKeyData.length()-5) + ")";
                else
                    myNoKeyData = "''";

                TriggerData << "INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_key,audit_insdeldata) VALUES (ts,'INSERT',us,'" + query.value(0).toString() + "'," + mykeyData + "," + myNoKeyData + ");";

                TriggerData << "";
                TriggerData << "END$$";
                TriggerData << "DELIMITER ;";
                TriggerData << "";




                //Delete trigger for MySQL----------------------------------------------------------------------------

                dropMyTriggers << "DROP TRIGGER audit_" + QString::number(auditIndex) +"_delete;";

                TriggerData << "delimiter $$";
                TriggerData << "CREATE TRIGGER audit_" + QString::number(auditIndex) +"_delete";
                TriggerData << "AFTER DELETE ON " + query.value(0).toString();
                TriggerData << "FOR EACH ROW BEGIN";
                TriggerData << "DECLARE ts TIMESTAMP DEFAULT CURRENT_TIMESTAMP();";
                TriggerData << "DECLARE us VARCHAR(120) DEFAULT USER();";
                TriggerData << "";

                //Getting the key data of the deleted record
                sql = "SELECT clm_cod FROM dict_clminfo WHERE tbl_cod = '"  + query.value(0).toString() + "' AND clm_key = 1";
                if (query2.exec(sql))
                {
                    mykeyData = "CONCAT(";
                    while (query2.next())
                    {
                        mykeyData = mykeyData + "'(" + query2.value(0).toString() + ")',OLD." + query2.value(0).toString() + ",',',";
                    }
                }
                mykeyData = mykeyData.left(mykeyData.length()-5) + ")";

                // Getting the non-key data of the deleted record
                // The non-key data is represented as Hexadecimal values to avoid conflicts with controlling characters like () and ,
                sql = "SELECT clm_cod FROM dict_clminfo WHERE tbl_cod = '"  + query.value(0).toString() + "' AND clm_key = 0";
                if (query2.exec(sql))
                {
                    myNoKeyData = "CONCAT(";
                    while (query2.next())
                    {
                        myNoKeyData = myNoKeyData + "'(" + query2.value(0).toString() + ")',ifnull(hex(cast(OLD." + query2.value(0).toString() + " as char)),''),',',";
                    }
                }
                if (myNoKeyData != "CONCAT(")
                    myNoKeyData = myNoKeyData.left(myNoKeyData.length()-5) + ")";
                else
                    myNoKeyData = "''";


                TriggerData << "INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_key,audit_insdeldata) VALUES (ts,'DELETE',us,'" + query.value(0).toString() + "'," + mykeyData + "," + myNoKeyData + ");";

                TriggerData << "";
                TriggerData << "END$$";
                TriggerData << "DELIMITER ;";
                TriggerData << "";





                //----------------------------------------------------------------------SQLite------------------------------------------------------------------------------------------------------------------------------

                //Update trigger for SQLite------------------------------------------------------------------

                QString litekeyData;
                QString liteNoKeyData;
                litekeyData = "(";

                sql = "SELECT clm_cod FROM dict_clminfo WHERE tbl_cod = '"  + query.value(0).toString() + "' AND clm_key = 1";
                if (query2.exec(sql))
                {
                    while (query2.next())
                    {
                        litekeyData = litekeyData + "'(" + query2.value(0).toString() + ")' || OLD." + query2.value(0).toString() + "|| ',' ||";
                    }
                    litekeyData = litekeyData.left(litekeyData.length()-9) + ")";
                }

                dropLiteTriggers << "DROP TRIGGER audit_" + QString::number(auditIndex) + "_update;";

                TriggerLite << "CREATE TRIGGER audit_" + QString::number(auditIndex) + "_update";
                TriggerLite << "AFTER UPDATE ON " + query.value(0).toString();
                TriggerLite << "FOR EACH ROW BEGIN";
                TriggerLite << "";
                sql = "SELECT clm_cod FROM dict_clminfo WHERE tbl_cod = '"  + query.value(0).toString() + "'  ORDER BY clm_key,clm_pos DESC"; //AND clm_key = 0;
                if (query2.exec(sql))
                {
                    while (query2.next())
                    {
                        TriggerLite << "INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) values (hex(datetime('now','localtime')),'UPDATE','NONE','" + query.value(0).toString() + "','" + query2.value(0).toString() + "'," + litekeyData + ",OLD." + query2.value(0).toString() + ",NEW." + query2.value(0).toString() + ");";
                    }
                }

                TriggerLite << "";
                TriggerLite << "END;";
                TriggerLite << "";

                //Insert trigger for SQLite---------------------------------------------------------------------------

                dropLiteTriggers << "DROP TRIGGER audit_" + QString::number(auditIndex) + "_insert;";

                TriggerLite << "CREATE TRIGGER audit_" + QString::number(auditIndex) + "_insert";
                TriggerLite << "AFTER INSERT ON " + query.value(0).toString();
                TriggerLite << "FOR EACH ROW BEGIN";
                TriggerLite << "";

                //Getting the key data of the deleted record
                sql = "SELECT clm_cod FROM dict_clminfo WHERE tbl_cod = '"  + query.value(0).toString() + "' AND clm_key = 1";
                if (query2.exec(sql))
                {
                    litekeyData = "(";
                    while (query2.next())
                    {
                        litekeyData = litekeyData + "'(" + query2.value(0).toString() + ")' || NEW." + query2.value(0).toString() + "|| ',' ||";
                    }
                }
                litekeyData = litekeyData.left(litekeyData.length()-9) + ")";

                // Getting the non-key data of the deleted record
                // The non-key data is represented as Hexadecimal values to avoid conflicts with controlling characters like () and ,
                sql = "SELECT clm_cod FROM dict_clminfo WHERE tbl_cod = '"  + query.value(0).toString() + "' AND clm_key = 0";
                if (query2.exec(sql))
                {
                    liteNoKeyData = "(";
                    while (query2.next())
                    {
                        liteNoKeyData = liteNoKeyData + "'(" + query2.value(0).toString() + ")' || hex(NEW." + query2.value(0).toString() + ") || ',' ||";
                    }
                }
                if (liteNoKeyData != "(")
                    liteNoKeyData = liteNoKeyData.left(liteNoKeyData.length()-9) + ")";
                else
                    liteNoKeyData = "''";

                TriggerLite << "INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_key,audit_insdeldata) values (hex(datetime('now','localtime')),'INSERT','NONE','" + query.value(0).toString() + "'," + litekeyData + "," + liteNoKeyData + ");";

                TriggerLite << "";
                TriggerLite << "END;";
                TriggerLite << "";



                //Delete trigger for SQLite----------------------------------------------------------------------------------

                dropLiteTriggers << "DROP TRIGGER audit_" + QString::number(auditIndex) + "_delete;";

                TriggerLite << "CREATE TRIGGER audit_" + QString::number(auditIndex) + "_delete";
                TriggerLite << "AFTER DELETE ON " + query.value(0).toString();
                TriggerLite << "FOR EACH ROW BEGIN";
                TriggerLite << "";

                //Getting the key data of the deleted record
                sql = "SELECT clm_cod FROM dict_clminfo WHERE tbl_cod = '"  + query.value(0).toString() + "' AND clm_key = 1";
                if (query2.exec(sql))
                {
                    litekeyData = "(";
                    while (query2.next())
                    {
                        litekeyData = litekeyData + "'(" + query2.value(0).toString() + ")' || OLD." + query2.value(0).toString() + "|| ',' ||";
                    }
                }
                litekeyData = litekeyData.left(litekeyData.length()-9) + ")";

                // Getting the non-key data of the deleted record
                // The non-key data is represented as Hexadecimal values to avoid conflicts with controlling characters like () and ,
                sql = "SELECT clm_cod FROM dict_clminfo WHERE tbl_cod = '"  + query.value(0).toString() + "' AND clm_key = 0";
                if (query2.exec(sql))
                {
                    liteNoKeyData = "(";
                    while (query2.next())
                    {
                        liteNoKeyData = liteNoKeyData + "'(" + query2.value(0).toString() + ")' || hex(OLD." + query2.value(0).toString() + ") || ',' ||";
                    }
                }
                if (liteNoKeyData != "(")
                    liteNoKeyData = liteNoKeyData.left(liteNoKeyData.length()-9) + ")";
                else
                    liteNoKeyData = "''";

                TriggerLite << "INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_key,audit_insdeldata) values (hex(datetime('now','localtime')),'DELETE','NONE','" + query.value(0).toString() + "'," + litekeyData + "," + liteNoKeyData + ");";

                TriggerLite << "";
                TriggerLite << "END;";
                TriggerLite << "";






            }
        }

        TriggerLite << "CREATE TRIGGER noAuditUpdates BEFORE UPDATE ON audit_log FOR EACH ROW BEGIN SELECT CASE WHEN ((SELECT NULL) IS NULL) THEN RAISE(ABORT, 'Audit table cannot be updated') END; END;";
        TriggerLite << "";

        TriggerLite << "CREATE TRIGGER noAuditDeletes BEFORE DELETE ON audit_log FOR EACH ROW BEGIN SELECT CASE WHEN ((SELECT NULL) IS NULL) THEN RAISE(ABORT, 'Audit table cannot be deleted') END; END;";
        TriggerLite << "";

        TriggerLite << "COMMIT;";
        TriggerLite << "";

        QDir audir(auditDir);
        if (!audir.exists())
        {
            audir.setPath(".");
            if (!audir.mkdir(auditDir))
            {
                log("Error creating audit dir");
                return 1;
            }
            else
            {
                audir.cd(auditDir);
            }

        }

        //Saves the MySQL create audit file
        QString fileName;
        fileName = audir.absolutePath() + "/" + "mysql_create_audit.sql";
        QFileInfo f(fileName);
        QString fp;
        fp = f.path();
        if (!fileName.isEmpty())
        {
            QFile myfile(fileName);
            if (!myfile.open(QIODevice::WriteOnly | QIODevice::Text))
                return 1;

            QTextStream myout(&myfile);
            for (int pos = 0; pos <= TriggerData.count() -1;pos++)
                myout << TriggerData[pos] << "\n";
        }
        log("MySQL Create audit script loaded in " + fileName);

        //Saves the SQLite create audit file
        fileName = "";
        fileName = audir.absolutePath() + "/" + "sqlite_create_audit.sql";

        if (!fileName.isEmpty())
        {
            QFile litefile(fileName);
            if (!litefile.open(QIODevice::WriteOnly | QIODevice::Text))
                return 1;

            QTextStream liteout(&litefile);
            for (int pos = 0; pos <= TriggerLite.count() -1;pos++)
                liteout << TriggerLite[pos] << "\n";
        }
        log("SQLite Create audit script loaded in " + fileName);

        //Saves the MySQL drop audit file
        fileName = "";
        fileName = audir.absolutePath() + "/" + "mysql_drop_audit.sql";
        if (!fileName.isEmpty())
        {
            QFile mydropfile(fileName);
            if (!mydropfile.open(QIODevice::WriteOnly | QIODevice::Text))
                return 1;

            QTextStream mydropout(&mydropfile);
            for (int pos = 0; pos <= dropMyTriggers.count() -1;pos++)
                mydropout << dropMyTriggers[pos] << "\n";
        }
        log("MySQL Drop audit script loaded in " + fileName);

        //Saves the SQlite drop audit file
        fileName = "";
        fileName = audir.absolutePath() + "/" + "sqlite_drop_audit.sql";
        if (!fileName.isEmpty())
        {
            QFile litedropfile(fileName);
            if (!litedropfile.open(QIODevice::WriteOnly | QIODevice::Text))
                return 1;

            QTextStream litedropout(&litedropfile);
            for (int pos = 0; pos <= dropLiteTriggers.count() -1;pos++)
                litedropout << dropLiteTriggers[pos] << "\n";
        }
        log("SQLite Drop audit script loaded in " + fileName);
    }
    return 0;
}

int loadMETATables(QSqlDatabase mydb, bool deletePrevious, bool includeViews)
{
    QSqlQuery query(mydb);
    QStringList sqls;

    int pos;
    bool error;
    error = false;

    if (deletePrevious)
    {

        sqls << "DELETE FROM dict_lkpiso639";
        sqls << "DELETE FROM dict_dctiso639";
        sqls << "DELETE FROM dict_iso639";
        sqls << "DELETE FROM audit_log";
        sqls << "DELETE FROM dict_childinfo";
        sqls << "DELETE FROM dict_relinfo";
        sqls << "DELETE FROM dict_clminfo";
        sqls << "DELETE FROM user_log";
        sqls << "DELETE FROM auth_user_user_permissions";
        sqls << "DELETE FROM auth_user_groups";
        sqls << "DELETE FROM auth_user";
        sqls << "DELETE FROM auth_group_permissions";
        sqls << "DELETE FROM auth_permission";
        sqls << "DELETE FROM dict_tblinfo";
        sqls << "DELETE FROM dict_grpinfo";
        sqls << "DELETE FROM auth_group";


        for (pos = 0; pos <= sqls.count()-1;pos++)
        {
            query.exec(sqls[pos]);
        }
    }

    QString sqlStr;
    QSqlQuery query2(mydb);
    QSqlQuery query3(mydb);
    QSqlQuery query4(mydb);

    //Start of procees

    if (!includeViews)
        sqlStr = "select table_name from information_schema.tables where table_schema = '" + mydb.databaseName() + "' and table_type = 'BASE TABLE'";
    else
        sqlStr = "select table_name from information_schema.tables where table_schema = '" + mydb.databaseName() + "'";
    query.exec(sqlStr);
    QString tablename;
    QString fieldname;
    QString fieldtype;
    bool mainDesc;
    bool key;
    int poscol;
    int tbllkp;
    while (query.next())
    {
        tablename = query.value(0).toString().trimmed();
        if (!isIgnoreTable(tablename))
        {
            if (tablename.toLower().left(3) == "lkp")
                tbllkp = 1;
            else
                tbllkp = 0;
            sqlStr = "INSERT INTO dict_tblinfo (tbl_cod,tbl_des,tbl_lkp,tbl_pos) VALUES (";
            sqlStr = sqlStr + "'" + tablename.toLower() + "',";
            sqlStr = sqlStr + "'Description of " + tablename + "',";
            sqlStr = sqlStr + "" + QString::number(tbllkp) + "," + QString::number(tables.indexOf(tablename)) + ")";
            if (!query2.exec(sqlStr))
            {
                log("Error inserting in Table Information");
                log(query2.lastError().databaseText());                
            }

            sqlStr = "desc " + tablename;
            query3.exec(sqlStr);
            poscol = 0;
            mainDesc = false;
            while (query3.next())
            {
                poscol++;
                fieldname = query3.value(0).toString().trimmed();
                fieldtype = query3.value(1).toString().trimmed();
                if (query3.value(3).toString().trimmed() == "PRI")
                    key = true;
                else
                    key = false;

                sqlStr = "INSERT INTO dict_clminfo (tbl_cod,clm_cod,clm_pos,clm_des,clm_key,clm_typ,clm_maindesc) VALUES (";
                sqlStr = sqlStr + "'" + tablename.toLower() + "',";
                sqlStr = sqlStr + "'" + fieldname + "',";
                sqlStr = sqlStr + QString::number(poscol) + ",";
                sqlStr = sqlStr + "'Description of " + fieldname + "',";
                if (key)
                    sqlStr = sqlStr + "1,";
                else
                    sqlStr = sqlStr + "0,";
                sqlStr = sqlStr + "'" + fieldtype + "',";
                if (mainDesc == false)
                {
                    if (fieldname.contains("_des") || fieldname.contains("_name"))
                    {
                        mainDesc = true;
                        sqlStr = sqlStr + "1)";
                    }
                    else
                    {
                        sqlStr = sqlStr + "0)";
                    }
                }
                else
                    sqlStr = sqlStr + "0)";

                if (!query4.exec(sqlStr))
                {
                    log("Error inserting in Column Information");
                    log(query4.lastError().databaseText());                    
                }

            }

        }
    }

    if (!includeViews)
        sqlStr = "select table_name from information_schema.tables where table_schema = '" + mydb.databaseName() + "' and table_type = 'BASE TABLE'";
    else
        sqlStr = "select table_name from information_schema.tables where table_schema = '" + mydb.databaseName() + "'";

    query.exec(sqlStr);
    QString errorMsg;
    while (query.next())
    {
        tablename = query.value(0).toString().trimmed();
        if (!isIgnoreTable(tablename))
        {
            sqlStr = "select table_name,column_name,REFERENCED_TABLE_NAME,";
            sqlStr = sqlStr + "REFERENCED_COLUMN_NAME,CONSTRAINT_NAME from ";
            sqlStr = sqlStr + "information_schema.KEY_COLUMN_USAGE where ";
            sqlStr = sqlStr + "table_schema = '" + mydb.databaseName() + "' and ";
            sqlStr = sqlStr + "table_name = '" + tablename + "' and ";
            sqlStr = sqlStr + "REFERENCED_TABLE_NAME is not null";

            //qDebug() << sqlStr;

            query3.exec(sqlStr);
            while (query3.next())
            {
                sqlStr = "INSERT INTO dict_relinfo (tbl_cod,clm_cod,rtbl_cod,rclm_cod,cnt_name,error_msg,error_notes) VALUES (";
                sqlStr = sqlStr + "'" + query3.value(0).toString().trimmed().toLower() + "',";
                sqlStr = sqlStr + "'" + query3.value(1).toString().trimmed() + "',";
                sqlStr = sqlStr + "'" + query3.value(2).toString().trimmed().toLower() + "',";
                sqlStr = sqlStr + "'" + query3.value(3).toString().trimmed() + "',";
                sqlStr = sqlStr + "'" + query3.value(4).toString().trimmed() + "',";

                errorMsg = "The value in column \"" + query3.value(1).toString().trimmed() + "\" is not included in the list of possible values in table \"" + query3.value(2).toString().trimmed() + "\"";
                errorMsg = errorMsg + ". Correct the value in table \"" + query3.value(0).toString().trimmed() + "\" or add the value to table \"" + query3.value(2).toString().trimmed() + "\"";

                sqlStr = sqlStr + "'" + errorMsg + "','')";

                if (!query4.exec(sqlStr))
                {
                    log("Error inserting in the Relation Table");
                    log(query4.lastError().databaseText());
                }
            }
        }
    }

    if (!error)
        log("META's tables sucessfully loaded");
    return 0;
}

int createMETATables(QSqlDatabase mydb)
{
    QSqlQuery query(mydb);
    QStringList sqls;
    QString sql;

    QFile file(":/model/model.sql");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        log("Error: Cannot open model file");
        return 1;
    }

    QTextStream in(&file);

    sql = "";
    while (!in.atEnd())
    {
        QString line = in.readLine().simplified();
        if (line.left(2) != "--")
        {
            if (!line.isEmpty())
            {
                if (line.contains(";"))
                {
                    if (sql.isEmpty())
                        sqls << line;
                    else
                        sql = sql + line + " ";
                }
                else
                {
                    sql = sql + line + " ";
                }
                if (sql.contains(";"))
                {
                    sqls << sql;
                    sql = "";
                }
            }
        }
    }

    int pos;
    bool error;
    error = false;
    for (pos = 0; pos <= sqls.count()-1;pos++)
    {
        if (!query.exec(sqls[pos]))
        {
            error = true;
            log("Error: " + query.lastError().databaseText());
            log("Executing: " + sqls[pos]);
            return 1;
        }
    }
    if (!error)
        log("META tables successfully created");
    return 0;
}

//Fillup the QStringList of tables in insertion order
int getTableOrder(QSqlDatabase db)
{
    QSqlQuery qtables(db);
    QString sql;
    //Get all the tables that are parents
    sql = "SELECT DISTINCT REFERENCED_TABLE_NAME FROM information_schema.KEY_COLUMN_USAGE";
    sql = sql + " WHERE table_schema = '" + db.databaseName() + "' AND";
    sql = sql + " REFERENCED_TABLE_NAME is not null";

    if (qtables.exec(sql))
    {
        while (qtables.next())
        {
            tables.append(qtables.value(0).toString());
        }
    }
    else
        return 1;

    //Get only neccesary references to workout the order in parent tables that are also parents
    sql = "SELECT table_name,REFERENCED_TABLE_NAME FROM";
    sql = sql + " information_schema.KEY_COLUMN_USAGE WHERE";
    sql = sql + " table_schema = '" + db.databaseName() + "' AND";
    sql = sql + " REFERENCED_TABLE_NAME is not null AND";
    sql = sql + " table_name IN (SELECT DISTINCT REFERENCED_TABLE_NAME FROM information_schema.KEY_COLUMN_USAGE";
    sql = sql + " WHERE table_schema = '" + db.databaseName() + "' AND";
    sql = sql + " REFERENCED_TABLE_NAME is not null)";
    sql = sql + " group by table_name,REFERENCED_TABLE_NAME";
    references = QSqlQuery(db);
    references.exec(sql);

    //Because a parent table can also be a child we need to organize the list so childs are after parents
    int pos;
    int parentIndex;
    QString table;
    for (pos = 0; pos <= tables.count()-1; pos++)
    {
        //Get the maximum index if the parent of this table
        parentIndex = getMaxParentIndex(tables[pos]);
        if (pos < parentIndex) //If the position of this table is before the max parent index
        {
            table = tables[pos]; //Get the table name
            tables.removeAt(pos); //Remove the table from the list
            tables.insert(parentIndex+1,table); //Insert it back in the position after the last parent
            pos = 0; //Back to beginning of the process
        }

    }

    //Append the tables that have no childs
    sql = "SELECT DISTINCT information_schema.tables.table_name FROM information_schema.tables WHERE";
    sql = sql + " information_schema.tables.table_schema = '" + db.databaseName() + "' AND";
    sql = sql + " table_type = 'BASE TABLE' AND";
    sql = sql + " information_schema.tables.table_name NOT IN";
    sql = sql + " (SELECT DISTINCT REFERENCED_TABLE_NAME FROM information_schema.KEY_COLUMN_USAGE";
    sql = sql + " WHERE table_schema = '" + db.databaseName() + "' AND";
    sql = sql + " REFERENCED_TABLE_NAME is not null)";
    if (qtables.exec(sql))
    {
        while (qtables.next())
        {
            tables.append(qtables.value(0).toString());
        }
    }
    return 0;
}


int main(int argc, char *argv[])
{
    QString title;
    title = title + "****************************************************************** \n";
    title = title + " * MySQLtoFile 1.0                                                * \n";
    title = title + " * This tool exports a table in a MySQL schema to various file    * \n";
    title = title + " * formats like STATA, SPSS, CSV (tab delimited), JSON and XML    * \n";
    title = title + " * This tool is part of CSPro Tools (c) ILRI, 2013                * \n";
    title = title + " ****************************************************************** \n";

    TCLAP::CmdLine cmd(title.toUtf8().data(), ' ', "1.0 (Beta 1)");
    //Required arguments
    TCLAP::ValueArg<std::string> hostArg("H","host","MySQL host. Default localhost",false,"localhost","string");
    TCLAP::ValueArg<std::string> portArg("P","port","MySQL port. Default 3306.",false,"3306","string");
    TCLAP::ValueArg<std::string> userArg("u","user","User to connect to MySQL",true,"","string");
    TCLAP::ValueArg<std::string> passArg("p","password","Password to connect to MySQL",true,"","string");
    TCLAP::ValueArg<std::string> schemaArg("s","schema","Schema in MySQL",true,"","string");
    TCLAP::ValueArg<std::string> auditArg("a","audit","Target directory for the audit files. Default ./audit (created if not exists)",false,"./audit","string");


    TCLAP::SwitchArg createSwitch("c","create","Create META dictionary tables", cmd, false);
    TCLAP::SwitchArg loadSwitch("l","load","Load META dictionary tables", cmd, false);
    TCLAP::SwitchArg deleteSwitch("d","delete","Delete previous data before load", cmd, false);
    TCLAP::SwitchArg viewSwitch("v","views","Include views in dictionary tables", cmd, false);


    cmd.add(hostArg);
    cmd.add(portArg);
    cmd.add(userArg);
    cmd.add(passArg);
    cmd.add(schemaArg);
    cmd.add(auditArg);

    cmd.parse( argc, argv );

    //Getting the variables from the command

    bool createMETA;
    bool loadMETA;
    bool deldata;
    bool includeViews;
    createMETA = createSwitch.getValue();
    loadMETA = loadSwitch.getValue();
    deldata = deleteSwitch.getValue();
    includeViews = viewSwitch.getValue();

    QString host = QString::fromUtf8(hostArg.getValue().c_str());
    QString port = QString::fromUtf8(portArg.getValue().c_str());
    QString user = QString::fromUtf8(userArg.getValue().c_str());
    QString pass = QString::fromUtf8(passArg.getValue().c_str());
    QString schema = QString::fromUtf8(schemaArg.getValue().c_str());
    QString auditPath = QString::fromUtf8(auditArg.getValue().c_str());


    //Dictionary tables
    ignoreTables << "dict_grpinfo";
    ignoreTables << "dict_tblinfo";
    ignoreTables << "dict_clminfo";
    ignoreTables << "dict_relinfo";
    ignoreTables << "dict_childinfo";
    ignoreTables << "audit_log";


    //Authorisation tables Not used since 2015-03-19. Backward compatibility
    ignoreTables << "auth_group";
    ignoreTables << "auth_group_permissions";
    ignoreTables << "auth_permission";
    ignoreTables << "auth_user";
    ignoreTables << "auth_user_groups";
    ignoreTables << "auth_user_user_permissions";
    ignoreTables << "user_log";


    //Django tables Not used since 2014. Backward compatibility
    ignoreTables << "auth_message";
    ignoreTables << "django_admin_log";
    ignoreTables << "django_content_type";
    ignoreTables << "django_session";
    ignoreTables << "django_site";

    //I18N tables
    ignoreTables << "dict_iso639";
    ignoreTables << "dict_lkpiso639";
    ignoreTables << "dict_dctiso639";



    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
        db.setHostName(host);
        db.setPort(port.toInt());
        db.setDatabaseName(schema);
        db.setUserName(user);
        db.setPassword(pass);
        bool METACreated;
        METACreated = false;
        bool METALoaded;
        METALoaded = false;
        bool deletePrevious;
        deletePrevious = false;
        QString data;
        if (db.open())
        {
            getTableOrder(db);
            if (createMETA)
            {
                log("Are you sure that you want to create META's tables? This will delete any previous META information INCLUDING the audit. (Y/N): ",false);
                QTextStream createIn(stdin);
                data = createIn.readLine();
                if (data.toUpper().simplified() == "Y")
                {
                    if (createMETATables(db) == 0)
                        METACreated = true;
                    else
                        return 1;
                }
            }
            if (loadMETA)
            {
                if (deldata && !createMETA)
                {
                    log("Are you sure that you want to delete any previous META's data? This WILL NOT delete the audit. (Y/N): ",false);
                    QTextStream deleteIn(stdin);
                    data = deleteIn.readLine();
                    if (data.toUpper().simplified() == "Y")
                    {
                        deletePrevious = true;
                    }
                }
                if (loadMETATables(db,deletePrevious,includeViews) == 0)
                    METALoaded = true;
                else
                    return 1;
            }
            createMETAAudit(db,auditPath);
            if (METACreated && !METALoaded)
            {
                log("META was created but not information was loaded. Use initmeta -l to load METAS's information");
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
