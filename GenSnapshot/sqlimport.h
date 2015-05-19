#ifndef SQLIMPORT_H
#define SQLIMPORT_H

#include <QThread>
#include <QMutex>
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QVector>


struct sqlIndexStr
{
  QString tableName;
  QString indexName;
  QString indexFields;
};
typedef sqlIndexStr tsqlIndex;

struct sqlFieldInfo
{
  QString fieldName;
  bool fieldAcceptNull;
};
typedef sqlFieldInfo tsqlFieldInfo;

struct sqlConstraintStr
{
  QString tableName;
  QString constraintName;
  //QStringList foreignKeysFields;
  QVector<tsqlFieldInfo> foreignKeysFields;
  QVector<tsqlFieldInfo> foreignKeyRefFields;
  QString referenceTable;
  QStringList referenceFields;
  QString onDelete;
  QString onUpdate;
};
typedef sqlConstraintStr tsqlConstraint;

/*
    The main idea of this class is to separate the insert from SQLite to MySQL in a different execution thread.
    Having these processes in a differen thread allows a proper response to the user
*/
class sqlimport : public QThread
{
Q_OBJECT
public:
    explicit sqlimport(QObject *parent = 0); //Constructor
    ~sqlimport(); //Destructor
    void run(); //The main thread process
    QString inputFile;
    QString outputFile;
    bool translateConstraints;
    bool processInserts;
    bool warnings;

signals:
    void processingLine(int bytes); //This signal is not in use at the moment.

public slots:
    void stopProcess();

private:
    bool m_abort;
    QMutex mutex;
    QMutex *filemutex;
    void process_line(QString sqlLine);
    void processSQL(QStringList sqlLines);
    QFile outfile;
    QTextStream outStream;
    QString cleanLine(QString sqlLine);

    QString cleanSQL(QString sqlStatement);

    QString usedDB;
    QStringList SQLStatement;
    bool collectSQL;
    QVector<tsqlIndex> indexes;
    QVector<tsqlConstraint> constraints;

    void processKeyIndexes(QString tablename,QString sqlStatement,int startPos);
    void processIndexes(QString tablename,QString sqlStatement,int startPos);
    void processConstraints(QString tablename,QString sqlStatement,int startPos);

    void storeIndex(QString tablename, QString indexStatement);
    void storeConstraint(QString tablename, QString constraintStatement);


    QVector<tsqlFieldInfo> separateFields(QString fields);
    QStringList separateFields2(QString fields);

    QString removeIndexes(QString sqlStatement);
    QString removeConstraints(QString sqlStatement);

    //void processInsert(QString insertStatement,int startPos,QString InsertHead);

    void processInsert(QString insertStatement);

    void setNULLToForeignKeys(QString tablename,QString sqlStatement);

    void writeToFile(QByteArray data);

    bool isSemiColonInsideSingleQuote();
};

#endif // SQLIMPORT_H
