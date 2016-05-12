#include <stdlib.h>
#include <QCoreApplication>
#include <tclap/CmdLine.h>
#include <QDir>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QVariant>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QStringList>

bool includeProtected;

int getFieldIndex(QSqlQuery query,QString fieldName)
{
    return query.record().indexOf(fieldName); //Return the index of fieldName;
}

void log(QString message)
{
    QString temp;
    temp = "Error:" + message + "\n";
    printf(temp.toLocal8Bit().data());
}

int STATALabelIndex;

int exportToXML(QSqlQuery dict, QSqlQuery data, QString table, QString target)
{
    QFile file(target);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        log("Cannot open file " + target);
        return 1;
    }

    QTextStream out(&file);
    out.setCodec("UTF-8");
    out << "<?xml version=\'1.0\'encoding=\'UTF-8\'?>"<< "\n";
    out<<"<"<< table <<">"<< "\n";

    QString str;
    QString field;
    QString value;
    QString type;
    int record =1;

    while (data.next())
    {
        dict.first();
        out<<"\t"<<"<row number=\""<<QString::number(record)<<"\">"<<"\n";
        str="";
        while (dict.isValid())
        {
            field = dict.value(0).toString();
            value = data.value(getFieldIndex(data,field)).toString().trimmed();            
            type = dict.value(2).toString();
            if(value.isNull())
            {
                str = str + "\t\t" + "<field name=\"" + field + "\" type=" + "\"" + type + "\" xsi:nil=\"true\"/>"+"\n";
            }
            else
            {
                str = str + "\t\t" + "<field name=\"" + field + "\" type=" + "\"" + type + "\">" + value  + "</field >" + "\n";
            }
            dict.next();
        }
        out<<str;
        out<<"\t"<<"</row>"<<"\n";
        record++;
    }
    out<<"</"<< table <<">"<< "\n";

    file.close();

    return 0;
}

int exportToJSON(QSqlQuery dict, QSqlQuery data, QString target) {
    QFile file(target);

    //Open text file for writing
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        log("Cannot open file " + target);
        return 1;
    }

    QTextStream out(&file);
    out.setCodec("UTF-8");

    //Loop through the data and process each row
    QString str;

    while (data.next()) {
        str = "{";

        //Using the dictionary get each of the fields in the current data record / row
        dict.first();
        QString field;
        QString value;

        while (dict.isValid()) {
            field = dict.value(0).toString();
            str = str + "\"" + field + "\":";

            //Get the value in the current field
            if (!data.value(getFieldIndex(data, field)).isNull())
                value = data.value(getFieldIndex(data, field)).toString();
            else
                value = "null";

            //Use the data type to determine if to put quotes on the value
            if (dict.value(2).toString().toLower().indexOf("varchar") >= 0) {
                str = str + "\"" + value + "\"";
            }
            else if (dict.value(2).toString().toLower().indexOf("int") >= 0) {
                str = str + value;
            }
            else if (dict.value(2).toString().toLower().indexOf("decimal") >= 0) {
                str = str + value;
            }
            else {
                str = str + "\"" + value + "\"";
            }

            str = str + ",";

            dict.next();
        }

        //Remove the last comma in the string
        str = str.left(str.length()-1) + "}";
        str = str.replace("\"null\"","null");
        //Write the JSON string to the file
        out << str << "\n";
    }

    //Close the file
    file.close();

    return 0;
}

int exportToSQL(QSqlDatabase db, QSqlQuery dict, QSqlQuery data, QString target, QString table)
{
    QSqlQuery dict1(db);
    QString sql1;
    QString createFile;
    QString insertFile;
    QString pos;
    sql1="SELECT tbl_pos from dict_tblinfo where tbl_cod = '" + table + "'";
    dict1.exec(sql1);
    dict1.first();
    pos = dict1.value(0).toString();

    if (target[target.length()-1] == '/') {
        createFile = target + pos + "-" + table + "-insert.sql";
        insertFile = target + pos + "-" + table + "-insert.sql";
    }
    else {
        createFile = target + "/" + pos + "-" + table + "-create.sql";
        insertFile = target + "/" + pos + "-" + table + "-insert.sql";
    }

    QFile file(createFile);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        log("Cannot open file " + target);
        return 1;
    }

    QTextStream out(&file);
    out.setCodec("UTF-8");
    dict.first();

    QString str1;
    str1 = "START TRANSACTION;\n\nCREATE TABLE " + table + "(\n";

    while (dict.isValid()) {
        str1 = str1 + dict.value(0).toString() + " " + dict.value(2).toString() + ",\n";
        dict.next();
    }

    str1 = str1.left(str1.length() - 2);
    str1 = str1 + "\n);\n\n" + "COMMIT;\n";
    out <<str1;
    dict.first();

    QFile file2(insertFile);
    if (!file2.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        log("Cannot open file " + target);
        return 1;
    }

    QTextStream cout(&file2);
    cout.setCodec("UTF-8");
    dict.first();

    QString field;
    QString value;
    QString str;
    QString str2;

    while (dict.isValid())
    {
        str2 = str2 + dict.value(0).toString() + ",";
        dict.next();

    }

    str2 = str2.left(str2.length()-1);
    dict.first();

    cout <<"START TRANSACTION;\n\n";

    while (data.next())
    {
        dict.first();
        str = "INSERT INTO "+table+" ("+str2+") VALUES (";
        while (dict.isValid())
        {
            field = dict.value(0).toString();

            if (!data.value(getFieldIndex(data,field)).isNull())
                value = data.value(getFieldIndex(data,field)).toString();
            else
                value = "NULL";

            if (dict.value(2).toString().toLower().indexOf("varchar") >= 0) {
                str = str + "'" + value + "'";
            }
            else if (dict.value(2).toString().toLower().indexOf("int") >= 0) {
                str = str + value;
            }
            else if (dict.value(2).toString().toLower().indexOf("decimal") >= 0) {
                str = str + value;
            }
            else {
                str = str + "'" + value + "'";
            }

            str = str + ",";
            dict.next();
        }
        str = str.left(str.length()-1);
        str = str.replace("'NULL'","NULL");
        cout << str <<");\n";
    }
    cout << "\nCOMMIT;";
    file.close();
    file2.close();
    return 0;
}

QString getRelatedTable(QSqlDatabase db,QString table, QString field)
{
    QSqlQuery dict(db);
    QString sql;
    sql = "SELECT rtbl_cod FROM dict_relinfo WHERE tbl_cod = '" + table + "' AND clm_cod = '" + field + "'";
    if (dict.exec(sql))
    {
        if (dict.first())
            return " (see " + dict.value(0).toString() + ")";
        else
            return "";
    }
    else
        return "";
}

int exportToCSV(QSqlDatabase db,QString table, QSqlQuery dict, QSqlQuery data, QString target, int DictColCount)
{
    QFile file(target);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        log("Cannot open file " + target);
        return 1;
    }

    //log("Parsing heading for table " + table);

    QTextStream out(&file);
    out.setCodec("UTF-8");
    dict.first();
    QString str;
    QString value;
    while (dict.isValid())
    {
        str = str + "\"" + dict.value(1).toString() + getRelatedTable(db,table,dict.value(0).toString()) + "\",";
        dict.next();
    }
    str = str.left(str.length()-1);
    out << str << "\n";
    int col;

    while (data.next())
    {
        str = "";
        for (col = 0; col <= DictColCount-1;col++)
        {
            if (!data.value(col).isNull())
            {
                value = data.value(col).toString();
                value = value.replace("\n"," - ");
                str = str + "\"" + value + "\",";
            }
            else
                str = str + "\"\",";
        }
        str = str.left(str.length()-1);
        out << str << "\n";
    }

    file.close();

    return 0;
}

QString cleanString(QString value)
{
    QString res;
    res = value;
    res = res.replace('\"',"");
    res = res.replace("'","");
    return res;
}

QString fixString(QString value, int length, bool isNull = false)
{
    int temp;
    if (!isNull)
        temp = length - value.trimmed().length();
    else
    {
        if (value == " ")
            temp = length - 1;
        else
            temp = length - value.trimmed().length();
    }
    QString spaces;
    for (int pos = 1; pos <= temp; pos++)
        spaces = spaces + " ";
    return value.trimmed() + spaces;
}

int getLength(QString type)
{
    if ((type.toLower() != "date") && (type.toLower() != "time") && (type.toLower() != "datetime") && (type.toLower() != "timestamp") && (type.toLower() != "year"))
    {
        int pos,pos2;
        pos = type.indexOf("(");
        pos2 = type.indexOf(")");
        if (pos2 > pos)
        {
            QString size;
            size = type.mid(pos+1,pos2-pos-1);
            //qDebug() << size;
            pos = size.indexOf(",");
            if (pos > 0)
            {
                size = size.left(pos);
                return size.toInt();
            }
            else
                return size.toInt();
        }
        else
        {
            return 255; //Some max value here
            log("Max value 255 assumed");
        }
    }
    else
    {
        if (type.toLower() == "date")
            return 10;
        else if (type.toLower() == "time")
            return 8;
        else if (type.toLower() == "datetime")
            return 19;
        else if (type.toLower() == "timestamp")
            return 19;
        else
            return 4;
    }
}

bool isFieldRelated(QSqlDatabase db, QString table,QString field)
{
    QString sql;
    sql = "select dict_relinfo.tbl_cod,dict_relinfo.clm_cod,dict_relinfo.rtbl_cod,dict_relinfo.rclm_cod from dict_relinfo,dict_tblinfo where ";
    sql = sql + "dict_relinfo.rtbl_cod = dict_tblinfo.tbl_cod and ";
    sql = sql + "dict_tblinfo.tbl_lkp = 1 and ";
    sql = sql + "dict_relinfo.tbl_cod = '" + table + "' AND ";
    sql = sql + "dict_relinfo.clm_cod = '" + field + "'";

    QSqlQuery rels(db);
    if (rels.exec(sql)) //Select the fields that are related to lookups
    {
        if (rels.first())
            return true;
    }
    return false;
}

bool isFieldAlfa(QSqlQuery dict, QString field)
{
    dict.first();
    while (dict.isValid())
    {
        if (dict.value(0).toString() == field)
        {
            if (dict.value(2).toString().toLower().indexOf("varchar") >= 0)
                return true;
            if (dict.value(2).toString().toLower().indexOf("date") >= 0)
                return true;
            if (dict.value(2).toString().toLower().indexOf("time") >= 0)
                return true;

        }
        dict.next();
    }
    return false;
}

QString fixField(QString field)
{
    int idx;
    idx = field.indexOf("_labels_");
    if (idx >= 0)
    {
        return field.right((field.length()-idx)-1);
    }
    else
        return field;
}

int exportToSTATA(QSqlDatabase db, QSqlQuery dict, QSqlQuery data, QString table, QString target, QString nullValue,int DictColCount)
{
    QString dir;
    if ((target[target.length()-1] == '/') || (target[target.length()-1] == '\\'))
        dir = target;
    else
        dir = target + "/";

    //Creates the data file.
    QFile file(dir + table + ".dat");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        log("Cannot open file " + target + table + ".dat");
        return 1;
    }

    QTextStream out(&file);
    out.setCodec("UTF-8");
    QString str;
    QString value;
    QString field;
    int size;

    QStringList sizes;
    dict.first();
    while (dict.isValid())
    {
        sizes.append(QString::number(getLength(dict.value(2).toString())));
        dict.next();
    }

    while (data.next())
    {
        dict.first();
        str = "";

        for (int col = 0; col <= DictColCount-1;col++)
        {
            size = sizes.at(col).toInt();
            if (!data.value(col).isNull())
            {
                value = data.value(col).toString().trimmed();
                value = value.replace("\n"," - ");
                str = str + fixString(value,size);
            }
            else
            {
                str = str + fixString(nullValue,size);
            }
        }        
        out << str << '\n';
    }
    file.close();

    //Creates the dct file

    file.setFileName(dir + table + ".dct");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        log("Cannot open file " + target + table + ".dct");
        return 1;
    }
    out << "infix dictionary using \"" << table + ".dat\" {" << "\n";
    out << "1 lines" << "\n";
    int start;
    start = 0;
    dict.first();
    QString stataType;
    while (dict.isValid())
    {
        start ++;
        size = getLength(dict.value(2).toString());        
        field = dict.value(0).toString();
        field = fixField(field);



        stataType = "str";
        //if (dict.value(2).toString().toLower().indexOf("varchar") >= 0)
        //{
            //if (!isFieldRelated(db,table,field))
            //    stataType = "str";
            //else
            //    stataType = "int";
        //}
        if (dict.value(2).toString().toLower().indexOf("int") >= 0)
            stataType = "long";
        if (dict.value(2).toString().toLower().indexOf("decimal") >= 0)
            stataType = "float";
        str = "    " + fixString(stataType,9) + fixString(field,field.length()+4) + "1:  " + QString::number(start) + "-" + QString::number(start+(size-1));
        out << str << "\n";
        start = start + (size-1);
        dict.next();
    }
    out << "}" << "\n";
    file.close();


    if (!QFile::exists(dir + "set-working-dir.do"))
    {
        file.setFileName(dir + "set-working-dir.do");
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            log("Cannot open file " + target + "set-working-dir.do");
            return 1;
        }

        out << "/* Change c:\\my_working_dir to the directory holding the files and execute this file before any other file */" <<  "\n";
        out << "cd \"c:\\my_working_dir\"" <<  "\n";

        file.close();
    }

    //Creates the do file

    file.setFileName(dir + table + ".do");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        log("Cannot open file " + target + table + ".do");
        return 1;
    }
    out << "/* Set ther working directory by editing and excuting the file set-working-dir.do */" <<  "\n";
    out << "infix using \"" << table + ".dct\"" << "\n";
    out << "\n";
    dict.first();
    while (dict.isValid())
    {
        if (dict.value(1).toString() != "")
            str = "label variable " + fixField(dict.value(0).toString()) + " \"" + dict.value(1).toString() + "\"";
        else
            str = "label variable " + fixField(dict.value(0).toString()) + " \"" + fixField(dict.value(0).toString()) + "\"";
        out << str << "\n";
        dict.next();
    }
    out << "\n";


    QString sql;
    sql = "select dict_relinfo.tbl_cod,dict_relinfo.clm_cod,dict_relinfo.rtbl_cod,dict_relinfo.rclm_cod from dict_relinfo,dict_tblinfo where ";
    sql = sql + "dict_relinfo.rtbl_cod = dict_tblinfo.tbl_cod and ";
    sql = sql + "dict_tblinfo.tbl_lkp = 1 and ";
    sql = sql + "dict_relinfo.tbl_cod = '" + table + "'";

    QStringList lkpdefines;
    QStringList lkplinks;

    QSqlQuery rels(db);
    QSqlQuery lkpdata(db);
    QString lkpvalue;
    int hasrels;
    hasrels = false;

    if (rels.exec(sql)) //Select the fields that are related to lookups
    {
        while (rels.next())
        {

            if (!isFieldAlfa(dict,rels.value(1).toString()))
            {
                if (field == "prodsys")
                    qDebug() << "Not alfa prodsys";

                hasrels = true;
                STATALabelIndex++;
                //lkplinks << "label values " + rels.value(1).toString() + " LBL" + rels.value(1).toString().toUpper() + "\n";
                lkplinks << "label values " + fixField(rels.value(1).toString()) + " LBL" + QString::number(STATALabelIndex) + "\n";

                //lkpdefines << "label define LBL" + rels.value(1).toString().toUpper() + "\n";
                lkpdefines << "label define LBL" + QString::number(STATALabelIndex) + "\n";

                field = rels.value(3).toString();
                field = field.left(field.length()-3) + "des";
                sql = "SELECT " + rels.value(3).toString() + "," + field + " FROM " + rels.value(2).toString();
                if (lkpdata.exec(sql))
                {
                    while (lkpdata.next())
                    {
                        lkpvalue = lkpdata.value(1).toString();
                        lkpvalue = lkpvalue.replace("\"","");
                        lkpvalue = lkpvalue.replace(";","");
                        lkpvalue = lkpvalue.replace("'","");

                        lkpdefines << "  " + lkpdata.value(0).toString() + " \"" + lkpvalue + "\"" + "\n";
                    }
                }
                lkpdefines << + ";\n";
            }
        }
        if (hasrels)
        {
            out << "#delimit ;" << "\n";
            lkpdefines << "\n";
            lkpdefines << + "#delimit cr\n";
        }
    }
    int pos;
    for (pos = 0; pos <= lkpdefines.count()-1;pos++)
    {
        out << lkpdefines[pos];
    }

    for (pos = 0; pos <= lkplinks.count()-1;pos++)
    {
        out << lkplinks[pos];
    }

    out << "\n" << "save " + table << "\n"; //Saves the table as a dta

    file.close();
    return 0;
}



int exportToSPSS(QSqlDatabase db, QSqlQuery dict, QSqlQuery data, QString table, QString target, QString nullValue,int DictColCount)
{
    QString dir;
    if ((target[target.length()-1] == '/') || (target[target.length()-1] == '\\'))
        dir = target;
    else
        dir = target + "/";

    //Creates the data file.
    QFile file(dir + table + ".dat");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        log("Cannot open file " + target + table + ".dat");
        return 1;
    }

    QTextStream out(&file);
    out.setCodec("UTF-8");
    QString str;
    QString value;
    QString field;

    QStringList sizes;
    dict.first();
    while (dict.isValid())
    {
        sizes.append(QString::number(getLength(dict.value(2).toString())));
        dict.next();
    }

    int size;
    while (data.next())
    {
        dict.first();
        str = "";

        for (int col = 0; col <= DictColCount-1;col++)
        {
            size = sizes.at(col).toInt();
            if (!data.value(col).isNull())
            {
                value = data.value(col).toString().trimmed();
                value = value.replace("\n"," - ");
                str = str + fixString(value,size);
            }
            else
            {
                str = str + fixString(nullValue,size);
            }
            dict.next();
        }        
        out << str << '\n';
    }
    file.close();

    //Creates the dct file

    if (!QFile::exists(dir + "set-working-dir.sps"))
    {
        file.setFileName(dir + "set-working-dir.sps");
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            log("Cannot open file " + target + "set-working-dir.sps");
            return 1;
        }

        out << "COMMENT Change c:\\my_working_dir to the directory holding the files and execute this file before any other file." <<  "\n";
        out << "CD 'c:\\my_working_dir'." <<  "\n";

        file.close();
    }


    file.setFileName(dir + table + ".sps");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        log("Cannot open file " + target + table + ".sps");
        return 1;
    }
    out << "COMMENT Set ther working directory by editing and excuting the file set-working-dir.sps." <<  "\n";
    out << "DATA LIST FILE='" << table + ".dat' RECORDS=1" << "\n";
    out << " /" << "\n";
    int start;
    start = 0;
    dict.first();
    QString SPSSType;
    while (dict.isValid())
    {
        start ++;
        size = getLength(dict.value(2).toString());
        field = dict.value(0).toString();
        field = fixField(field);
        SPSSType = "(A)";
        if (dict.value(2).toString().toLower().indexOf("varchar") >= 0)
        {
            SPSSType = "(A)";
        }
        if (dict.value(2).toString().toLower().indexOf("int") >= 0)
            SPSSType = "";
        if (dict.value(2).toString().toLower().indexOf("decimal") >= 0)
            SPSSType = "(3)";
        str = " " + field + " " + QString::number(start) + "-" + QString::number(start+(size-1)) + " " + SPSSType;
        out << str << "\n";
        start = start + (size-1);
        dict.next();
    }
    out << "." << "\n";
    out << "VARIABLE LABELS" << "\n";

    bool first;
    first = true;
    dict.first();
    while (dict.isValid())
    {
        if (first)
        {
            if ( dict.value(1).toString() != "")
                str = "  " + fixField(dict.value(0).toString()) + " \"" + cleanString(dict.value(1).toString()) + "\"";
            else
                str = "  " + fixField(dict.value(0).toString()) + " \"" + fixField(cleanString(dict.value(0).toString())) + "\"";
            first = false;
        }
        else
            if (dict.value(1).toString() != "")
                str = " /" + fixField(dict.value(0).toString()) + " \"" + cleanString(dict.value(1).toString()) + "\"";
            else
                str = " /" + fixField(dict.value(0).toString()) + " \"" + fixField(cleanString(dict.value(0).toString())) + "\"";

        out << str << "\n";
        dict.next();
    }
    out << "." << "\n";
    out << "VALUE LABELS" << "\n";


    //Processing the values

    QString sql;
    sql = "select dict_relinfo.tbl_cod,dict_relinfo.clm_cod,dict_relinfo.rtbl_cod,dict_relinfo.rclm_cod from dict_relinfo,dict_tblinfo where ";
    sql = sql + "dict_relinfo.rtbl_cod = dict_tblinfo.tbl_cod and ";
    sql = sql + "dict_tblinfo.tbl_lkp = 1 and ";
    sql = sql + "dict_relinfo.tbl_cod = '" + table + "'";


    QSqlQuery rels(db);
    QSqlQuery lkpdata(db);
    QString lkpvalue;

    bool alfa;
    first = true;
    if (rels.exec(sql)) //Select the fields that are related to lookups
    {
        while (rels.next())
        {
            field = rels.value(1).toString();
            field = fixField(field);
            if (first)
            {
                out << "  " + field + "\n";
                first = false;
            }
            else
                out << " /" + field + "\n";

            alfa = isFieldAlfa(dict,field);

            field = rels.value(3).toString();
            field = field.left(field.length()-3) + "des";
            sql = "SELECT " + rels.value(3).toString() + "," + field + " FROM " + rels.value(2).toString();
            if (lkpdata.exec(sql))
            {
                while (lkpdata.next())
                {
                    lkpvalue = lkpdata.value(1).toString();
                    lkpvalue = lkpvalue.replace("\"","");
                    lkpvalue = lkpvalue.replace(";","");
                    lkpvalue = lkpvalue.replace("'","");

                    if (!alfa)
                        out << "  " + lkpdata.value(0).toString() + " \"" + cleanString(lkpvalue) + "\"" + "\n";
                    else
                        out << "  '" + lkpdata.value(0).toString() + "' \"" + cleanString(lkpvalue) + "\"" + "\n";
                }
            }

        }
    }
    out << "." << "\n";

    out << "SAVE OUTFILE \"" + table + ".sav\"." << "\n";

    out << "EXECUTE." << "\n";

    file.close();
    return 0;
}

QStringList ignoreTables;


int exportTable(QSqlDatabase db, QString table, QString format, QString target, bool all, bool lookUpTable, QString nullValue)
{
    QSqlQuery dict(db);
    QSqlQuery query(db);
    QString sql;
    int colCount;
    colCount = 0;

    if (includeProtected)
        sql = "SELECT clm_cod,clm_des,clm_typ FROM dict_clminfo WHERE tbl_cod = '" + table + "' ORDER BY clm_pos";
    else
        sql = "SELECT clm_cod,clm_des,clm_typ FROM dict_clminfo WHERE tbl_cod = '" + table + "' AND clm_protected = 0 ORDER BY clm_pos";
    if (dict.exec(sql))
    {
        if (dict.next())
        {
            dict.first();
            sql = "SELECT ";
            colCount = 0;
            while (dict.isValid())
            {
                sql = sql + dict.value(0).toString() + ",";
                dict.next();
                colCount++;
            }
            sql = sql.left(sql.length()-1);
            sql = sql + " FROM " + table;
            if (query.exec(sql))
            {
                if (format == "CSV")
                {
                    if ((target[target.length()-1] == '/') || (target[target.length()-1] == '\\'))
                        target = target + table + ".csv";
                    else
                        target = target + "/" + table + ".csv";
                    if (exportToCSV(db,table,dict,query,target,colCount) > 0)
                    {
                        return 1;
                    }
                }
                if (format == "STATA")
                {
                    if (!lookUpTable)
                    {
                        exportToSTATA(db,dict,query,table,target,nullValue,colCount);
                    }
                }
                if (format == "SPSS")
                {
                    if (!lookUpTable)
                    {
                        exportToSPSS(db,dict,query,table,target,nullValue,colCount);
                    }
                }
                if (format == "SQL")
                {
                    if (exportToSQL(db,dict,query,target,table) > 0)
                    {
                        return 1;
                    }
                }
                if (format == "JSON")
                {
                    //Export to JSON format
                    if (target[target.length()-1] == '/') {
                        target = target + table + ".json";
                    }
                    else {
                        target = target + "/" + table + ".json";
                    }

                    if (exportToJSON(dict, query, target) > 0) {
                        return 1;
                    }
                }
                if (format == "XML")
                {
                    if ((target[target.length()-1] == '/') || (target[target.length()-1] == '\\'))
                        target = target + table + ".xml";
                    else
                        target = target + "/" + table + ".xml";
                    if (exportToXML(dict,query,table,target) > 0)
                    {
                        return 1;
                    }
                }

            }
            else
            {
                log("Cannot query table " + table);
                log(query.lastError().databaseText());
                return 1;
            }
        }
        else
        {
            if (!all)
            {
                log("Table " + table + " is not in the dictionary");
                return 1;
            }
        }
    }
    else
    {
        log("Cannot read the dictonary tables");
        return 1;
    }

    return 0;
}

bool isLookUpTable(QSqlDatabase db, QString table)
{
    QSqlQuery dict(db);
    if (dict.exec("select count(tbl_cod) FROM dict_tblinfo where tbl_cod = '" + table.toLower() + "' and tbl_lkp = 1"))
    {
        if (dict.first())
        {
            if (dict.value(0).toInt() == 1)
                return true;
            else
                return false;
        }
        else
            return false;

    }
    return false;
}

int main(int argc, char *argv[])
{
    STATALabelIndex = 0;
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
    TCLAP::ValueArg<std::string> tableArg("t","table","Table to export. Default ALL",false,"ALL","string");
    TCLAP::ValueArg<std::string> destArg("d","destination","Destination directory",true,"","string");
    TCLAP::ValueArg<std::string> outArg("o","output","Output type: STATA, SPSS, CSV (tab delimited), JSON or XML. Default CSV",false,"CSV","string");
    TCLAP::ValueArg<std::string> nullArg("n","nullValue","In case of NULL use this value. Default [space] for STATA and SPSS, 0 for others",false,"","string");

    TCLAP::SwitchArg remoteSwitch("T","protected","Include protected", cmd, false);


    cmd.add(hostArg);
    cmd.add(portArg);
    cmd.add(userArg);
    cmd.add(passArg);
    cmd.add(schemaArg);
    cmd.add(tableArg);
    cmd.add(destArg);
    cmd.add(outArg);
    cmd.add(nullArg);
    //Parsing the command lines
    cmd.parse( argc, argv );

    //Getting the variables from the command

    includeProtected = remoteSwitch.getValue();

    QString host = QString::fromUtf8(hostArg.getValue().c_str());
    QString port = QString::fromUtf8(portArg.getValue().c_str());
    QString user = QString::fromUtf8(userArg.getValue().c_str());
    QString pass = QString::fromUtf8(passArg.getValue().c_str());
    QString schema = QString::fromUtf8(schemaArg.getValue().c_str());
    QString table = QString::fromUtf8(tableArg.getValue().c_str());
    QString dest = QString::fromUtf8(destArg.getValue().c_str());
    QString output = QString::fromUtf8(outArg.getValue().c_str());
    QString nullValue = QString::fromUtf8(nullArg.getValue().c_str());

    if (!((output == "STATA") || (output == "SPSS") || (output == "CSV") || (output == "JSON") || (output == "XML") || (output == "SQL")))
    {
        log("Output type is not valid");
        return 1;
    }

    if (nullValue == "")
    {
        if ((output == "STATA") || (output == "SPSS"))
            nullValue = " ";
        else
            nullValue = "0";
    }

    QDir dir;
    if (!dir.exists(dest))
    {
        log("Destination directory does not exists");
        return 1;
    }


    //Dictionary tables
    ignoreTables << "dict_grpinfo";
    ignoreTables << "dict_tblinfo";
    ignoreTables << "dict_clminfo";
    ignoreTables << "dict_relinfo";
    ignoreTables << "dict_childinfo";
    ignoreTables << "audit_log";

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

    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
        db.setHostName(host);
        db.setPort(port.toInt());
        db.setDatabaseName(schema);
        db.setUserName(user);
        db.setPassword(pass);
        if (db.open())
        {
            if (table == "ALL")
            {
                QSqlQuery tables(db);
                if (tables.exec("select tbl_cod,tbl_lkp FROM dict_tblinfo"))
                {
                    int error;
                    error = 0;
                    while (tables.next())
                    {
                        if (ignoreTables.indexOf(tables.value(0).toString().toLower()) < 0)
                        {
                            if (exportTable(db,tables.value(0).toString().toLower(),output,dest,true,tables.value(1).toBool(),nullValue) > 0)
                            {
                                error = 1;
                            }
                        }
                    }
                    if (error)
                    {
                        db.close();
                        return 1;
                    }
                }
                else
                {
                    log("Cannot read tables in schema");
                    log(tables.lastError().databaseText());
                    db.close();
                    return 1;
                }
            }
            else
            {
                if (ignoreTables.indexOf(table.toLower()) < 0)
                {
                    if (exportTable(db,table,output,dest,false,isLookUpTable(db,table),nullValue) > 0)
                    {
                        db.close();
                        return 1;
                    }
                }
                else
                {
                    db.close();
                    log("Cannot export table " + table);
                    return 1;
                }
            }

            QSqlQuery query("SELECT NAME, DOB FROM EMPLOYEES", db);
            db.close();
        }
        else
        {
            log("Cannot connect to database");
            log(db.lastError().databaseText());
            return 1;
        }
    }

    qDebug() << "Done";
    return 0;
}
