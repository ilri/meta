#include "sqlimport.h"
#include <QDebug>
#include <QFile>
#include <QVector>

sqlimport::sqlimport(QObject *parent) :
    QThread(parent)
{
    m_abort = false;
    usedDB = "";
    translateConstraints = true;
    processInserts = false;
    warnings = false;
    filemutex = new QMutex(QMutex::Recursive);
}

sqlimport::~sqlimport()
{
    delete filemutex;
    filemutex = 0;
}

void sqlimport::writeToFile(QByteArray data)
{
    filemutex->lock();
    outfile.write(data);
    filemutex->unlock();
}

//This will stop the process
void sqlimport::stopProcess()
 {
     mutex.lock();
     m_abort = true;
     mutex.unlock();
 }

void sqlimport::run()
{
    int totalbytes;

    totalbytes = 0;


    outfile.setFileName(outputFile);
    if (!outfile.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    writeToFile(QString("BEGIN;\n").toUtf8());

    QFile file(inputFile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream input(&file);
    collectSQL = false;
    while (!input.atEnd())
    {
        QString sqlLine = input.readLine();

        totalbytes = totalbytes + sqlLine.toUtf8().size();

        process_line(sqlLine);
        emit processingLine(totalbytes);
        if (m_abort)
        {
            outfile.flush();
            outfile.close();
            file.close();
            return;
        }
    }

    //Flush the indexes at the end of the file



    QString indexStatement;
    indexStatement = "-- Indexes to be created by the convertion\n";

    //outfile.write(indexStatement.toAscii());
    writeToFile(indexStatement.toUtf8());

    int pos;


    for (pos = 0; pos <= indexes.count()-1;pos++)
    {
        indexStatement = "CREATE INDEX ";
        indexStatement = indexStatement + indexes[pos].tableName + "_" + indexes[pos].indexName;
        indexStatement = indexStatement + " ON " + indexes[pos].tableName;
        indexStatement = indexStatement + " " + indexes[pos].indexFields;
        indexStatement = indexStatement + ";\n";
        //outfile.write(indexStatement.toAscii());
        writeToFile(indexStatement.toUtf8());
    }

    if (translateConstraints)
    {
        indexStatement = "-- Triggers to be created by the convertion\n";
        //outfile.write(indexStatement.toAscii());
        writeToFile(indexStatement.toUtf8());

        QString trigger;
        int pos2;
        int pos3;

        //Creates INSERT constraint triggers based on original constraints

        indexStatement = "-- INSERT triggers\n";
        //outfile.write(indexStatement.toAscii());
        writeToFile(indexStatement.toUtf8());

        bool notNullForeignKey;

        for (pos = 0; pos <= constraints.count()-1;pos++)
        {
            // Checking the null status of the foreign keys
            // If one of them is NOT NULL treat the whole
            // constraint as not null

            if (constraints[pos].foreignKeysFields.count() == 1)
            {
                notNullForeignKey = !constraints[pos].foreignKeysFields[0].fieldAcceptNull;
            }
            else
            {
                notNullForeignKey = false;
                for (pos2 = 0;pos2 <= constraints[pos].foreignKeysFields.count()-1;pos2++)
                {
                    if (constraints[pos].foreignKeysFields[pos2].fieldAcceptNull == false)
                    {
                        notNullForeignKey = true;
                    }
                }
            }

            if (notNullForeignKey) //The foreign keys are NOT NULL
            {
                //Insert will pass if data is in referenced table

                trigger = "CREATE TRIGGER INS_" + constraints[pos].constraintName;
                trigger = trigger + " BEFORE INSERT ON " + constraints[pos].tableName;
                trigger = trigger + " FOR EACH ROW BEGIN";
                trigger = trigger + " SELECT CASE";
                trigger = trigger + " WHEN ((";

                trigger = trigger + "SELECT (";
                for (pos2 = 0;pos2 <= constraints[pos].referenceFields.count()-1;pos2++)
                {
                    trigger = trigger + constraints[pos].referenceFields[pos2] + " || ";
                }
                trigger = trigger.left(trigger.length()-4); //Remove the last ||
                trigger = trigger + ") as value FROM " + constraints[pos].referenceTable;
                trigger = trigger + " WHERE " ;
                trigger = trigger + constraints[pos].referenceFields[0];
                trigger = trigger + " = ";
                trigger = trigger + "NEW." + constraints[pos].foreignKeysFields[0].fieldName;
                //If there are more the one contraint
                if (constraints[pos].foreignKeysFields.count() > 1)
                {
                    for (pos2 = 1;pos2 <= constraints[pos].referenceFields.count()-1;pos2++)
                    {
                        trigger = trigger + " AND " ;
                        trigger = trigger + constraints[pos].referenceFields[pos2];
                        trigger = trigger + " = ";
                        trigger = trigger + "NEW." + constraints[pos].foreignKeysFields[pos2].fieldName;
                    }
                }

                trigger = trigger + ") IS NULL)";
                trigger = trigger + " THEN RAISE(ABORT, 'insert on table \"" + constraints[pos].tableName + "\" violates foreign key constraint \"" + constraints[pos].constraintName + "\"')";
                trigger = trigger + " END;";
                trigger = trigger + " END;\n";
                //Writes the trigger
                //outfile.write(trigger.toAscii());
                writeToFile(trigger.toUtf8());
            }
            else
            {
                // Will insert those lines when the values of the
                // foreign keys are NULL or valid

                trigger = "CREATE TRIGGER INS_" + constraints[pos].constraintName;
                trigger = trigger + " BEFORE INSERT ON " + constraints[pos].tableName;
                trigger = trigger + " FOR EACH ROW BEGIN";
                trigger = trigger + " SELECT CASE";
                trigger = trigger + " WHEN ((";

                for (pos2 = 0;pos2 <= constraints[pos].foreignKeysFields.count()-1;pos2++)
                {
                    trigger = trigger + "NEW." + constraints[pos].foreignKeysFields[pos2].fieldName;
                    trigger = trigger + " IS NOT NULL AND ";
                }
                trigger = trigger.left(trigger.length()-5); //Remove the last _AND_


                trigger = trigger + ") AND ((SELECT (";
                for (pos2 = 0;pos2 <= constraints[pos].referenceFields.count()-1;pos2++)
                {
                    trigger = trigger + constraints[pos].referenceFields[pos2] + " || ";
                }
                trigger = trigger.left(trigger.length()-4); //Remove the last ,
                trigger = trigger + ") as value FROM " + constraints[pos].referenceTable;
                trigger = trigger + " WHERE " ;
                trigger = trigger + constraints[pos].referenceFields[0];
                trigger = trigger + " = ";
                trigger = trigger + "NEW." + constraints[pos].foreignKeysFields[0].fieldName;
                //If there are more the one contraint
                if (constraints[pos].foreignKeysFields.count() > 1)
                {
                    for (pos2 = 1;pos2 <= constraints[pos].referenceFields.count()-1;pos2++)
                    {
                        trigger = trigger + " AND " ;
                        trigger = trigger + constraints[pos].referenceFields[pos2];
                        trigger = trigger + " = ";
                        trigger = trigger + "NEW." + constraints[pos].foreignKeysFields[pos2].fieldName;
                    }
                }

                trigger = trigger + ") IS NULL))";
                trigger = trigger + " THEN RAISE(ABORT, 'insert on table \"" + constraints[pos].tableName + "\" violates foreign key constraint \"" + constraints[pos].constraintName + "\"')";
                trigger = trigger + " END;";
                trigger = trigger + " END;\n";
                //Writes the trigger
                //outfile.write(trigger.toAscii());
                writeToFile(trigger.toUtf8());

            }
        }

        //Creates UPDATE constraint triggers based on original constraints

        indexStatement = "-- UPDATE triggers\n";
        //outfile.write(indexStatement.toAscii());
        writeToFile(indexStatement.toUtf8());

        for (pos = 0; pos <= constraints.count()-1;pos++)
        {
            // Checking the null status of the foreign keys
            // If one of them is NOT NULL treat the whole
            // constraint as not null

            if (constraints[pos].foreignKeysFields.count() == 1)
            {
                notNullForeignKey = !constraints[pos].foreignKeysFields[0].fieldAcceptNull;
            }
            else
            {
                notNullForeignKey = false;
                for (pos2 = 0;pos2 <= constraints[pos].foreignKeysFields.count()-1;pos2++)
                {
                    if (constraints[pos].foreignKeysFields[pos2].fieldAcceptNull == false)
                    {
                        notNullForeignKey = true;
                    }
                }
            }

            if (notNullForeignKey) //The foreign keys are NOT NULL
            {
                //Insert will pass if data is in referenced table

                trigger = "CREATE TRIGGER UPD_" + constraints[pos].constraintName;
                trigger = trigger + " BEFORE UPDATE ON " + constraints[pos].tableName;
                trigger = trigger + " FOR EACH ROW BEGIN";
                trigger = trigger + " SELECT CASE";
                trigger = trigger + " WHEN ((";

                trigger = trigger + "SELECT (";
                for (pos2 = 0;pos2 <= constraints[pos].referenceFields.count()-1;pos2++)
                {
                    trigger = trigger + constraints[pos].referenceFields[pos2] + " || ";
                }
                trigger = trigger.left(trigger.length()-4); //Remove the last ,
                trigger = trigger + ") as value FROM " + constraints[pos].referenceTable;
                trigger = trigger + " WHERE " ;
                trigger = trigger + constraints[pos].referenceFields[0];
                trigger = trigger + " = ";
                trigger = trigger + "NEW." + constraints[pos].foreignKeysFields[0].fieldName;
                //If there are more the one contraint
                if (constraints[pos].foreignKeysFields.count() > 1)
                {
                    for (pos2 = 1;pos2 <= constraints[pos].referenceFields.count()-1;pos2++)
                    {
                        trigger = trigger + " AND " ;
                        trigger = trigger + constraints[pos].referenceFields[pos2];
                        trigger = trigger + " = ";
                        trigger = trigger + "NEW." + constraints[pos].foreignKeysFields[pos2].fieldName;
                    }
                }

                trigger = trigger + ") IS NULL)";
                trigger = trigger + " THEN RAISE(ABORT, 'insert on table \"" + constraints[pos].tableName + "\" violates foreign key constraint \"" + constraints[pos].constraintName + "\"')";
                trigger = trigger + " END;";
                trigger = trigger + " END;\n";
                //Writes the trigger
                //outfile.write(trigger.toAscii());
                writeToFile(trigger.toUtf8());
            }
            else
            {
                // Will insert those lines when the values of the
                // foreign keys are NULL or valid

                trigger = "CREATE TRIGGER UPD_" + constraints[pos].constraintName;
                trigger = trigger + " BEFORE UPDATE ON " + constraints[pos].tableName;
                trigger = trigger + " FOR EACH ROW BEGIN";
                trigger = trigger + " SELECT CASE";
                trigger = trigger + " WHEN ((";

                for (pos2 = 0;pos2 <= constraints[pos].foreignKeysFields.count()-1;pos2++)
                {
                    trigger = trigger + "NEW." + constraints[pos].foreignKeysFields[pos2].fieldName;
                    trigger = trigger + " IS NOT NULL AND ";
                }
                trigger = trigger.left(trigger.length()-5); //Remove the last _AND_


                trigger = trigger + ") AND ((SELECT (";
                for (pos2 = 0;pos2 <= constraints[pos].referenceFields.count()-1;pos2++)
                {
                    trigger = trigger + constraints[pos].referenceFields[pos2] + " || ";
                }
                trigger = trigger.left(trigger.length()-4); //Remove the last ,
                trigger = trigger + ") as value FROM " + constraints[pos].referenceTable;
                trigger = trigger + " WHERE " ;
                trigger = trigger + constraints[pos].referenceFields[0];
                trigger = trigger + " = ";
                trigger = trigger + "NEW." + constraints[pos].foreignKeysFields[0].fieldName;
                //If there are more the one contraint
                if (constraints[pos].foreignKeysFields.count() > 1)
                {
                    for (pos2 = 1;pos2 <= constraints[pos].referenceFields.count()-1;pos2++)
                    {
                        trigger = trigger + " AND " ;
                        trigger = trigger + constraints[pos].referenceFields[pos2];
                        trigger = trigger + " = ";
                        trigger = trigger + "NEW." + constraints[pos].foreignKeysFields[pos2].fieldName;
                    }
                }

                trigger = trigger + ") IS NULL))";
                trigger = trigger + " THEN RAISE(ABORT, 'insert on table \"" + constraints[pos].tableName + "\" violates foreign key constraint \"" + constraints[pos].constraintName + "\"')";
                trigger = trigger + " END;";
                trigger = trigger + " END;\n";
                //Writes the trigger
                //outfile.write(trigger.toAscii());
                writeToFile(trigger.toUtf8());

            }
        }



        indexStatement = "-- UPDATE parent triggers and DELETE triggers\n";
        writeToFile(indexStatement.toUtf8());

        for (pos = 0; pos <= constraints.count()-1;pos++)
        {
            //Create the UPDATE constraint triggers
            //Create DELETE constraint trigger
            if (constraints[pos].onUpdate != "NA")
            {
                //--NO ACTION is treated the same as RESTRICT
                if ((constraints[pos].onUpdate.contains("RESTRICT")) ||
                        (constraints[pos].onUpdate.contains("NO ACTION")))
                {
                    //qDebug() << constraints[pos].constraintName;
                    //qDebug() << constraints[pos].referenceTable;
                    /*for (pos2 = 0;pos2 <= constraints[pos].foreignKeysFields.count()-1;pos2++)
                    {
                        qDebug() << constraints[pos].foreignKeyRefFields[pos2].fieldName;
                    }*/

                    for (pos2 = 0;pos2 <= constraints[pos].foreignKeysFields.count()-1;pos2++)
                    {
                        trigger = "CREATE TRIGGER UPD_" + constraints[pos].constraintName + "_" + constraints[pos].foreignKeysFields[pos2].fieldName;
                        trigger = trigger + " BEFORE UPDATE OF " + constraints[pos].foreignKeyRefFields[pos2].fieldName + " ON " + constraints[pos].referenceTable;
                        trigger = trigger + " FOR EACH ROW BEGIN";
                        trigger = trigger + " SELECT CASE";
                        trigger = trigger + " WHEN ((";

                        trigger = trigger + "SELECT (";
                        for (pos3 = 0;pos3 <= constraints[pos].foreignKeysFields.count()-1;pos3++)
                        {
                            trigger = trigger + constraints[pos].foreignKeysFields[pos3].fieldName + " || ";
                        }
                        trigger = trigger.left(trigger.length()-4); //Remove the last ,
                        trigger = trigger + ") as value FROM " + constraints[pos].tableName;
                        trigger = trigger + " WHERE " ;
                        trigger = trigger + constraints[pos].foreignKeysFields[0].fieldName;
                        trigger = trigger + " = ";
                        trigger = trigger + "OLD." + constraints[pos].referenceFields[0];
                        //If there are more the one contraint
                        if (constraints[pos].foreignKeysFields.count() > 1)
                        {
                            for (pos3 = 1;pos3 <= constraints[pos].referenceFields.count()-1;pos3++)
                            {
                                trigger = trigger + " AND " ;
                                trigger = trigger + constraints[pos].foreignKeysFields[pos3].fieldName;
                                trigger = trigger + " = ";
                                trigger = trigger + "OLD." + constraints[pos].referenceFields[pos3];
                            }
                        }

                        trigger = trigger + ") IS NOT NULL)";
                        trigger = trigger + " THEN RAISE(ABORT, 'update on table \"" + constraints[pos].referenceTable + "\" violates foreign key constraint \"" + constraints[pos].constraintName + "\"')";
                        trigger = trigger + " END;";
                        trigger = trigger + " END;\n";
                        //Writes the trigger
                        //outfile.write(trigger.toAscii());
                        writeToFile(trigger.toUtf8());

                    }
                }
            }



            //Create DELETE constraint trigger
            if (constraints[pos].onDelete != "NA")
            {


                //--NO ACTION is treated the same as RESTRICT
                if ((constraints[pos].onDelete.contains("RESTRICT")) ||
                        (constraints[pos].onDelete.contains("NO ACTION")))
                {
                    trigger = "CREATE TRIGGER DLT_" + constraints[pos].constraintName;
                    trigger = trigger + " BEFORE DELETE ON " + constraints[pos].referenceTable;
                    trigger = trigger + " FOR EACH ROW BEGIN";
                    trigger = trigger + " SELECT CASE";
                    trigger = trigger + " WHEN ((";

                    trigger = trigger + "SELECT (";
                    for (pos2 = 0;pos2 <= constraints[pos].foreignKeysFields.count()-1;pos2++)
                    {
                        trigger = trigger + constraints[pos].foreignKeysFields[pos2].fieldName + " || ";
                    }
                    trigger = trigger.left(trigger.length()-4); //Remove the last ,
                    trigger = trigger + ") as value FROM " + constraints[pos].tableName;
                    trigger = trigger + " WHERE " ;
                    trigger = trigger + constraints[pos].foreignKeysFields[0].fieldName;
                    trigger = trigger + " = ";
                    trigger = trigger + "OLD." + constraints[pos].referenceFields[0];
                    //If there are more the one contraint
                    if (constraints[pos].foreignKeysFields.count() > 1)
                    {
                        for (pos2 = 1;pos2 <= constraints[pos].referenceFields.count()-1;pos2++)
                        {
                            trigger = trigger + " AND " ;
                            trigger = trigger + constraints[pos].foreignKeysFields[pos2].fieldName;
                            trigger = trigger + " = ";
                            trigger = trigger + "OLD." + constraints[pos].referenceFields[pos2];
                        }
                    }

                    trigger = trigger + ") IS NOT NULL)";
                    trigger = trigger + " THEN RAISE(ABORT, 'delete on table \"" + constraints[pos].tableName + "\" violates foreign key constraint \"" + constraints[pos].constraintName + "\"')";
                    trigger = trigger + " END;";
                    trigger = trigger + " END;\n";
                    //Writes the trigger
                    //outfile.write(trigger.toAscii());
                    writeToFile(trigger.toUtf8());
                }
                if (constraints[pos].onDelete.contains("CASCADE"))
                {
                    //Constructs the trigger
                    trigger = "CREATE TRIGGER DLT_" + constraints[pos].constraintName;
                    trigger = trigger + " BEFORE DELETE ON " + constraints[pos].referenceTable;
                    trigger = trigger + " FOR EACH ROW BEGIN";
                    trigger = trigger + " DELETE FROM " + constraints[pos].tableName;
                    trigger = trigger + " WHERE " ;
                    trigger = trigger + constraints[pos].foreignKeysFields[0].fieldName;
                    trigger = trigger + " = ";
                    trigger = trigger + "OLD." + constraints[pos].referenceFields[0];
                    //If there are more the one contraint
                    if (constraints[pos].foreignKeysFields.count() > 1)
                    {
                        for (pos2 = 1;pos2 <= constraints[pos].referenceFields.count()-1;pos2++)
                        {
                            trigger = trigger + " AND " ;
                            trigger = trigger + constraints[pos].foreignKeysFields[pos2].fieldName;
                            trigger = trigger + " = ";
                            trigger = trigger + "OLD." + constraints[pos].referenceFields[pos2];
                        }
                    }
                    trigger = trigger + "; END;\n";
                    //Writes the trigger
                    //outfile.write(trigger.toAscii());
                    writeToFile(trigger.toUtf8());
                }
                if (constraints[pos].onDelete == "SET NULL")
                {
                    indexStatement = "-- ON DELETE SET NULL FOR table " + constraints[pos].tableName + " is not supported yet!\n";
                    //outfile.write(indexStatement.toAscii());
                    writeToFile(indexStatement.toUtf8());
                }
                /*if (constraints[pos].onDelete == "NO ACTION")
                {
                    indexStatement = "-- ON DELETE NO ACTION FOR table " + constraints[pos].tableName + " is not supported yet!\n";
                    //outfile.write(indexStatement.toAscii());
                    writeToFile(indexStatement.toAscii());
                }*/
            }
        }

    }
    writeToFile(QString("COMMIT;\n").toUtf8());
    outfile.flush();
    outfile.close();
    file.close();



    //End of the import process
}

bool sqlimport::isSemiColonInsideSingleQuote()
{
    int totsqts;
    totsqts = 0;
    QString statementNow;
    int posscolon;
    QString subString;
    for (int pos = 0;pos <= SQLStatement.count()-1; pos++)
    {
        statementNow = statementNow + SQLStatement[pos];
    }
    posscolon = statementNow.lastIndexOf(";");
    subString = statementNow.left(posscolon);

    //qDebug() << subString;

    totsqts = subString.count("'");
    if (totsqts > 0)
    {
        if (totsqts % 2 == 0)
            return false;
        else
            return true;
    }
    else
    {
        return false;
    }
}

void sqlimport::process_line(QString sqlLine)
{
    QString lineData;
    lineData = cleanLine(sqlLine);

    if (!lineData.isEmpty())
    {
        if ((lineData.left(13) == "CREATE  TABLE") ||
            (lineData.left(12) == "CREATE TABLE"))
        {
            SQLStatement.clear();
            collectSQL = false;
            if (lineData.lastIndexOf(";") >= 0)
            {
                SQLStatement << lineData;
                if (isSemiColonInsideSingleQuote() == false)
                {
                    collectSQL = false;
                    processSQL(SQLStatement);
                    SQLStatement.clear();
                }
                else
                {
                    collectSQL = true;
                }
            }
            else
            {
                SQLStatement << lineData;
                collectSQL = true;
            }
        }
        else
        {            
            if ((lineData.left(12) == "INSERT  INTO") ||
                (lineData.left(11) == "INSERT INTO"))
            {
                //qDebug() << lineData.left(11);
                SQLStatement.clear();
                collectSQL = false;
                if (lineData.lastIndexOf(";") >= 0)
                {
                    SQLStatement << lineData;
                    if (isSemiColonInsideSingleQuote() == false)
                    {
                        collectSQL = false;
                        processSQL(SQLStatement);
                        SQLStatement.clear();
                    }
                    else
                    {
                        collectSQL = true;
                    }
                }
                else
                {
                    SQLStatement << lineData;
                    collectSQL = true;
                }
            }
            else
            {
                if (lineData.lastIndexOf(";") >= 0)
                {
                    SQLStatement << lineData;
                    if (isSemiColonInsideSingleQuote() == false)
                    {
                        collectSQL = false;
                        processSQL(SQLStatement);
                        SQLStatement.clear();
                    }
                    else
                    {
                        //qDebug() << "Why!";
                        collectSQL = true;
                    }
                }
                else
                {
                    if (collectSQL)
                    {
                        SQLStatement << lineData;
                    }
                    else
                    {
                        lineData = lineData + "\n";
                        //outfile.write(lineData.toAscii());
                        writeToFile(lineData.toUtf8());
                        //outStream << lineData;
                    }
                }
            }
        }
    }
}


QString sqlimport::cleanLine(QString sqlLine)
{
    QString lineData;
    lineData = sqlLine;
    //qDebug() << "Before: " << lineData;
    if (lineData.left(5) == "SET @")
    {
        lineData = "";
        return lineData;
    }
    if (lineData.left(13) == "CREATE SCHEMA")
    {
        lineData = "";
        return lineData;
    }

    if (lineData.left(4) == "SET ")
    {
        lineData = "";
        return lineData;
    }

    if (lineData.left(9) == "ENGINE = ")
    {
        lineData = ";";
        return lineData;
    }

    if (lineData.left(3) == "/*!")
    {
        lineData = "";
        return lineData;
    }

    if (lineData.left(11) == "LOCK TABLES")
    {
        lineData = "";
        return lineData;
    }

    if (lineData.left(13) == "UNLOCK TABLES")
    {
        lineData = "";
        return lineData;
    }

    if (lineData.left(10) == "DROP TABLE")
    {
        lineData = "";
        return lineData;
    }

    if (lineData.left(5) == "USE `")
    {
        int start;
        int end;
        start = lineData.indexOf("`");
        end = lineData.lastIndexOf("`");
        usedDB = lineData.mid(start+1,end-start-1);
        lineData = "";
        return lineData;
    }
    lineData = lineData.replace("`","");

    if (!usedDB.isEmpty())
    {
        lineData = lineData.replace(usedDB+".","");
    }


    return lineData;
}

QString sqlimport::cleanSQL(QString sqlStatement)
{
    QString lineData;
    lineData = sqlStatement;

    if (lineData.indexOf("COMMENT=") >=0)
    {
        int temp;
        int temp2;
        temp = lineData.indexOf("COMMENT=");
        temp2 = lineData.indexOf("'",temp+9);
        lineData = lineData.remove(temp,temp2-temp+1);
    }

    if (lineData.indexOf("COMMENT =") >=0)
    {
        int temp;
        int temp2;
        temp = lineData.indexOf("COMMENT =");
        temp2 = lineData.indexOf("'",temp+11);
        lineData = lineData.remove(temp,temp2-temp+1);
    }


    int temp;
    int temp2;

    while (lineData.indexOf("COMMENT '") >= 0)
    {
        temp = lineData.indexOf("COMMENT '");
        temp2 = lineData.indexOf(",",temp+9);

        //Gets the , outside '   '

        QString subString;
        int numComas;
        subString = lineData.left(temp2);
        numComas = subString.count("'");

        if (numComas > 0)
        {
            while (numComas % 2 != 0)
            {
                temp2 = lineData.indexOf(",",temp2+1);
                subString = lineData.left(temp2);
                numComas = subString.count("'");
            }
        }
        lineData = lineData.remove(temp,temp2-temp);
    }


    if (lineData.indexOf("ENGINE=") >=0)
    {
        int temp;
        int temp2;
        temp = lineData.indexOf("ENGINE=");
        temp2 = lineData.indexOf(" ",temp+8);
        if (temp2 >=0)
        {
            lineData = lineData.remove(temp,temp2-temp+1);
        }
        else
        {
            temp2 = lineData.lastIndexOf(";");
            lineData = lineData.remove(temp,temp2-temp);
        }
    }

    if (lineData.indexOf("ROW_FORMAT =") >=0)
    {
        int temp;
        int temp2;
        temp = lineData.indexOf("ROW_FORMAT =");
        temp2 = lineData.indexOf(" ",temp+14);
        if (temp2 >=0)
        {
            lineData = lineData.remove(temp,temp2-temp+1);
        }
        else
        {
            temp2 = lineData.lastIndexOf(";");
            lineData = lineData.remove(temp,temp2-temp);
        }
    }

    if (lineData.indexOf("AUTO_INCREMENT=") >=0)
    {
        int temp;
        int temp2;
        temp = lineData.indexOf("AUTO_INCREMENT=");
        temp2 = lineData.indexOf(" ",temp+17);
        if (temp2 >=0)
        {
            lineData = lineData.remove(temp,temp2-temp+1);
        }
        else
        {
            temp2 = lineData.lastIndexOf(";");
            lineData = lineData.remove(temp,temp2-temp);
        }
    }

    if (lineData.indexOf("AUTO_INCREMENT =") >=0)
    {
        int temp;
        int temp2;
        temp = lineData.indexOf("AUTO_INCREMENT =");
        temp2 = lineData.indexOf(" ",temp+18);
        if (temp2 >=0)
        {
            lineData = lineData.remove(temp,temp2-temp+1);
        }
        else
        {
            temp2 = lineData.lastIndexOf(";");
            lineData = lineData.remove(temp,temp2-temp);
        }
    }

    if (lineData.indexOf("AUTO_INCREMENT") >=0)
    {
        QString message;
        warnings = true;
        message = "-- WARNING! Autoincrement was removed from the following CREATE TABLE statement. You may need to add it manually\n";
        lineData = lineData.replace("AUTO_INCREMENT","");
        //outfile.write(message.toAscii());
        writeToFile(message.toUtf8());
    }

    if (lineData.indexOf("DEFAULT CHARSET") >=0)
    {
        int temp;
        int temp2;
        temp = lineData.indexOf("DEFAULT CHARSET");
        temp2 = lineData.indexOf(" ",temp+18);
        if (temp2 >=0)
        {
            lineData = lineData.remove(temp,temp2-temp+1);
        }
        else
        {
            temp2 = lineData.lastIndexOf(";");
            lineData = lineData.remove(temp,temp2-temp);
        }
    }

    if (lineData.indexOf("DEFAULT CHARACTER SET") >=0)
    {
        int temp;
        int temp2;
        temp = lineData.indexOf("DEFAULT CHARACTER SET");
        temp2 = lineData.indexOf(" ",temp+25);
        if (temp2 >=0)
        {
            lineData = lineData.remove(temp,temp2-temp+1);
        }
        else
        {
            temp2 = lineData.lastIndexOf(";");
            lineData = lineData.remove(temp,temp2-temp);
        }
    }

    //Resolve Unique Key in MySQL moving to SQLite
    if (lineData.indexOf("UNIQUE KEY") >=0)
    {
        int temp;
        int temp2;
        temp = lineData.indexOf("UNIQUE KEY");
        temp2 = lineData.indexOf("(",temp);
        lineData = lineData.remove(temp+7,temp2-temp-7);
    }

    lineData = lineData.replace("IF NOT EXISTS ","");

    lineData = lineData.replace("ENGINE=InnoDB","",Qt::CaseInsensitive);

    lineData = lineData.replace("PACK_KEYS=0","",Qt::CaseInsensitive);
    lineData = lineData.replace("PACK_KEYS=1","",Qt::CaseInsensitive);

    if (lineData.indexOf(";") < 0)
    {
        lineData = lineData + ";";
    }

    return lineData;
}

void sqlimport::processSQL(QStringList sqlLines)
{
    QString sqlStatement;
    int nlines;
    QString tablename;
    int temp,temp2;

    for (nlines = 0; nlines <= sqlLines.count()-1;nlines++)
    {
        sqlStatement = sqlStatement + sqlLines[nlines].simplified() + " ";
    }

    //qDebug() << sqlStatement;

    if ((sqlStatement.toUpper().contains("CREATE TABLE ")) ||
            (sqlStatement.toUpper().contains("CREATE  TABLE ")))
    {
        //qDebug() << "CREATE:" << sqlStatement;
        sqlStatement = cleanSQL(sqlStatement);

        temp = sqlStatement.indexOf(" TABLE ");
        temp2 = sqlStatement.indexOf("(");


        tablename = sqlStatement.mid(temp+7,temp2-temp-8);

        processKeyIndexes(tablename,sqlStatement,0);
        processIndexes(tablename,sqlStatement,0);
        if (translateConstraints)
        {
            processConstraints(tablename,sqlStatement,0);
        }

        if (translateConstraints)
        {
            setNULLToForeignKeys(tablename,sqlStatement);
        }


        sqlStatement = sqlStatement.replace(" unsigned "," ",Qt::CaseInsensitive);

        sqlStatement = removeIndexes(sqlStatement);

        if (translateConstraints)
        {
            sqlStatement = removeConstraints(sqlStatement);
        }

        sqlStatement = sqlStatement + "\n";
        //outfile.write(sqlStatement.toAscii());
        writeToFile(sqlStatement.toUtf8());
    }
    else
    {
        if (processInserts == true)
        {
            //qDebug() << "INSERT:" << sqlStatement;
            //processInsert(sqlStatement,0,"");
            processInsert(sqlStatement);
        }
    }
}

void sqlimport::processInsert(QString insertStatement)
{
    int startPos;
    startPos = 0;

    int temp;
    int temp2;
    QString values;
    QString InsertHead;

    temp = insertStatement.indexOf("VALUES ");
    temp = insertStatement.indexOf("(",temp);
    InsertHead = insertStatement.left(temp-1);

    startPos = temp-1;

    while (insertStatement.indexOf("),(",startPos) >=0)
    {
        temp = insertStatement.indexOf("(",startPos);
        temp2 = insertStatement.indexOf("),(",temp);
        values = insertStatement.mid(temp,temp2-temp+1);
        values = InsertHead + values;
        values = values + ";\n";
        //outfile.write(values.toAscii());
        writeToFile(values.toUtf8());
        startPos = temp2+1;
    }
    //Last insert
    temp = insertStatement.indexOf("(",startPos);
    temp2 = insertStatement.lastIndexOf(")");
    values = insertStatement.mid(temp,temp2-temp+1);
    values = InsertHead + values;
    values = values + ";\n";
    //outfile.write(values.toAscii());
    writeToFile(values.toUtf8());
}

void sqlimport::processKeyIndexes(QString tablename,QString sqlStatement,int startPos)
{
    int posStart;
    int posEnd;
    QString indexStr;

    posStart = sqlStatement.indexOf(", KEY",startPos); //
    if (posStart >= 0)
    {
        posEnd = sqlStatement.indexOf(")",posStart+1);
        if (posEnd >= 0)
        {
            indexStr = sqlStatement.mid(posStart+1,posEnd-posStart);
            //Store the index
            storeIndex(tablename,indexStr.simplified());
            //Maybe there are more indexes so
            processKeyIndexes(tablename,sqlStatement,posEnd);
        }
        else
        {
            posEnd = sqlStatement.lastIndexOf(")",-1);
            indexStr = sqlStatement.mid(posStart+1,posEnd-posStart);
            //Store the index
            storeIndex(tablename,indexStr.simplified());
        }
    }
}

void sqlimport::processIndexes(QString tablename,QString sqlStatement,int startPos)
{
    int posStart;
    int posEnd;
    QString indexStr;
    posStart = sqlStatement.indexOf("INDEX ",startPos); //
    if (posStart >= 0)
    {
        posEnd = sqlStatement.indexOf(")",posStart+1);
        if (posEnd >= 0)
        {
            indexStr = sqlStatement.mid(posStart,posEnd-posStart+1); //+1 for INDEX?
            //Store the index
            storeIndex(tablename,indexStr.simplified());
            //Maybe there are more indexes so
            processIndexes(tablename,sqlStatement,posEnd);
        }
        else
        {
            posEnd = sqlStatement.lastIndexOf(")",-1);
            indexStr = sqlStatement.mid(posStart,posEnd-posStart+1); //+1 for INDEX?
            //Store the index
            storeIndex(tablename,indexStr.simplified());
        }
    }
}

void sqlimport::processConstraints(QString tablename,QString sqlStatement,int startPos)
{
    int posStart;
    int posEnd;
    QString constraintStr;

    posStart = sqlStatement.indexOf("CONSTRAINT ",startPos); //
    if (posStart >= 0)
    {
        posEnd = sqlStatement.indexOf("CONSTRAINT ",posStart+1);
        if (posEnd >= 0)
        {
            constraintStr = sqlStatement.mid(posStart,posEnd-posStart-1);
            //Store the constraint
            storeConstraint(tablename,constraintStr.simplified());
            //Maybe there are more constraints so
            processConstraints(tablename,sqlStatement,posEnd);
        }
        else
        {
            posEnd = sqlStatement.lastIndexOf(")",-1);
            constraintStr = sqlStatement.mid(posStart,posEnd-posStart);
            storeConstraint(tablename,constraintStr.simplified());
        }
    }
}

void sqlimport::storeIndex(QString tablename, QString indexStatement)
{
    QString indexName;
    QString indexFields;

    int temp;
    int temp2;
    int temp3;
    temp = indexStatement.indexOf(" ",0);
    temp2 = indexStatement.indexOf("(",0);
    temp3 = indexStatement.indexOf(")",0);

    indexName = indexStatement.mid(temp+1,temp2-temp-2);
    indexName = indexName.replace("-","_");

    indexFields = indexStatement.mid(temp2,temp3-temp2+1);

    tsqlIndex indexdef;
    indexdef.tableName = tablename;
    indexdef.indexName = indexName;
    indexdef.indexFields = indexFields;
    indexes.append(indexdef);

}

void sqlimport::storeConstraint(QString tablename, QString constraintStatement)
{

    tsqlConstraint constraintDef;

    int temp;
    int temp2;
    int temp3;

    constraintDef.tableName = tablename;

    temp = constraintStatement.indexOf("CONSTRAINT ");
    temp2 = constraintStatement.indexOf("FOREIGN KEY");
    constraintDef.constraintName = constraintStatement.mid(temp+11,temp2-temp-12);

    constraintDef.constraintName = constraintDef.constraintName.replace("-","_");

    temp = constraintStatement.indexOf("REFERENCES ");
    temp2 = constraintStatement.indexOf("(",temp);
    constraintDef.referenceTable = constraintStatement.mid(temp+11,temp2-temp-12);

    temp = constraintStatement.indexOf("(");
    temp2 = constraintStatement.indexOf(")");



    constraintDef.foreignKeysFields = separateFields(constraintStatement.mid(temp+1,temp2-temp-1));

    temp = constraintStatement.indexOf("REFERENCES ");
    temp = constraintStatement.indexOf("(",temp);
    temp2 = constraintStatement.indexOf(")",temp);


    constraintDef.foreignKeyRefFields = separateFields(constraintStatement.mid(temp+1,temp2-temp-1));


    //We need to move the fields to separate vars

    temp3 = constraintStatement.indexOf("REFERENCES ");
    temp = constraintStatement.indexOf("(",temp3);
    temp2 = constraintStatement.indexOf(")",temp3);
    constraintDef.referenceFields = separateFields2(constraintStatement.mid(temp+1,temp2-temp-1));

    //We need to move the fields to separate vars

    if (constraintStatement.contains("ON DELETE"))
    {
        temp = constraintStatement.indexOf("ON DELETE");
        temp2 = constraintStatement.indexOf(" ON ",temp+9);
        if (temp2 >= 0)
        {
            constraintDef.onDelete = constraintStatement.mid(temp+10,temp2-temp-10).simplified();
        }
        else
        {
            constraintDef.onDelete = constraintStatement.mid(temp+10,constraintStatement.length()-temp-10).simplified();
        }
    }
    else
    {        
        constraintDef.onDelete = "RESTRICT"; //Assuming restrict if not DELETE is found
    }

    if (constraintStatement.contains("ON UPDATE"))
    {
        temp = constraintStatement.indexOf("ON UPDATE");
        temp2 = constraintStatement.indexOf(" ON ",temp+9);
        if (temp2 >= 0)
        {
            constraintDef.onUpdate = constraintStatement.mid(temp+10,temp2-temp-10).simplified();
            //qDebug() << constraintDef.onUpdate;
        }
        else
        {            
            constraintDef.onUpdate = constraintStatement.mid(temp+10,constraintStatement.length()-temp-10).simplified();
            //qDebug() << constraintDef.onUpdate;
        }
    }
    else
    {
        //qDebug() << "Update restrict assumed";
        constraintDef.onUpdate = "RESTRICT"; //Assuming restrict if not UPDATE is found
    }

    constraints.append(constraintDef);
}

QStringList sqlimport::separateFields2(QString fields)
{
    QStringList res;
    QString stream;
    stream = fields.remove(" "); //Remove all the spaces
    QString field;
    field = "";
    for (int pos=0;pos<= stream.size()-1;pos++)
    {
        if (stream[pos] != ',')
        {
            field = field + stream[pos];
        }
        else
        {
            res << field;
            field = "";
        }
    }
    res << field;
    return res;
}

QVector<tsqlFieldInfo> sqlimport::separateFields(QString fields)
{
    QVector<tsqlFieldInfo> res;
    QString stream;
    stream = fields.remove(" "); //Remove all the spaces
    QString field;
    tsqlFieldInfo fieldInfo;
    field = "";
    for (int pos=0;pos<= stream.size()-1;pos++)
    {
        if (stream[pos] != ',')
        {
            field = field + stream[pos];
        }
        else
        {
            fieldInfo.fieldName = field;
            fieldInfo.fieldAcceptNull = true;
            res.append(fieldInfo);
            field = "";
        }
    }
    fieldInfo.fieldName = field;
    fieldInfo.fieldAcceptNull = true;
    res.append(fieldInfo);
    return res;
}


QString sqlimport::removeIndexes(QString sqlStatement)
{
    QString res;
    res = sqlStatement;

    int temp;
    int temp2;
    while (res.contains(", KEY"))
    {
        temp = res.indexOf(", KEY");
        temp2 = res.indexOf(", KEY",temp+5);
        if (temp2 >= 0)
        {
            res.remove(temp,temp2-temp);
        }
        else
        {
            temp2 = res.indexOf(")",temp);
            res.remove(temp,temp2-temp+1);
        }
    }
    while (res.contains(", INDEX "))
    {
        temp = res.indexOf(", INDEX ");
        temp2 = res.indexOf(", INDEX ",temp+5);
        if (temp2 >= 0)
        {
            res.remove(temp,temp2-temp);
        }
        else
        {
            temp2 = res.indexOf(")",temp);
            res.remove(temp,temp2-temp+1);
        }
    }
    return res;
}

QString sqlimport::removeConstraints(QString sqlStatement)
{
    QString res;
    res = sqlStatement;

    int temp;
    int temp2;
    while (res.contains(", CONSTRAINT "))
    {
        temp = res.indexOf(", CONSTRAINT ");
        temp2 = res.indexOf(", CONSTRAINT ",temp+5);
        if (temp2 >= 0)
        {
            res.remove(temp,temp2-temp);
        }
        else
        {
            temp2 = res.lastIndexOf(")");
            res.remove(temp,temp2-temp);
        }
    }
    return res;
}


void sqlimport::setNULLToForeignKeys(QString tablename,QString sqlStatement)
{
    int pos;
    int pos2;
    int temp;
    int temp2;
    QString fieldName;
    QString sqlSection;

    for (pos = 0; pos <= constraints.count()-1;pos++)
    {
        if (constraints[pos].tableName == tablename)
        {
            for (pos2 =0; pos2 <= constraints[pos].foreignKeysFields.count()-1;pos2++)
            {
                fieldName = constraints[pos].foreignKeysFields[pos2].fieldName;
                temp = sqlStatement.indexOf(fieldName);
                temp2 = sqlStatement.indexOf(", ",temp);
                if (temp2 >= 0)
                {
                    sqlSection = sqlStatement.mid(temp,temp2-temp);
                    if (sqlSection.contains("NOT NULL",Qt::CaseInsensitive))
                    {
                        constraints[pos].foreignKeysFields[pos2].fieldAcceptNull = false;
                    }
                }
                else
                {
                    temp2 = sqlStatement.indexOf(";");
                    sqlSection = sqlStatement.mid(temp,temp2-temp);
                    if (sqlSection.contains("NOT NULL",Qt::CaseInsensitive))
                    {
                        constraints[pos].foreignKeysFields[pos2].fieldAcceptNull = false;
                    }
                }
            }
        }
    }
}
