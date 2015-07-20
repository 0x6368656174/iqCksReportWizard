#include "sqllogdialog.h"
#include "ui_sqllogdialog.h"

SqlLogDialog::SqlLogDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SqlLogDialog),
    m_syntaxHighliter(new SqlTraceLogSyntaxHighliter(this))
{
    ui->setupUi(this);
    m_syntaxHighliter->setDocument(ui->sqlTraceLogTextEdit->document());
}

SqlLogDialog::~SqlLogDialog()
{
    delete ui;
}

void SqlLogDialog::setSqlOperationTracerLog(IqOrmSqlOperationTracerLog *sqlOperationTracerLog)
{
    if (sqlOperationTracerLog) {
#ifdef Q_OS_WIN
        connect(sqlOperationTracerLog, SIGNAL(beginTransaction(bool,QString)),
                this, SLOT(saveSqlBeginTransactionToTraceLog(bool,QString)));
        connect(sqlOperationTracerLog, SIGNAL(commitTransaction(bool,QString)),
                this, SLOT(saveSqlCommitTransactionToTraceLog(bool,QString)));
        connect(sqlOperationTracerLog, SIGNAL(rollbackTransaction(bool,QString)),
                this, SLOT(saveSqlRollbackTransactionToTraceLog(bool,QString)));
        connect(sqlOperationTracerLog, SIGNAL(prepareQuery(QString,qint32,bool,QString)),
                this, SLOT(saveSqlPrepereQueryToTraceLog(QString,qint32,bool,QString)));
        connect(sqlOperationTracerLog, SIGNAL(execQuery(QString,qint32,bool,QString)),
                this, SLOT(saveSqlExecQueryToTraceLog(QString,qint32,bool,QString)));
        connect(sqlOperationTracerLog, SIGNAL(execPreparedQuery(QVariantList,qint32,bool,QString)),
                this, SLOT(saveSqlExecPreparedQueryToTraceLog(QVariantList,qint32,bool,QString)));
#else
        connect(sqlOperationTracerLog, &IqOrmSqlOperationTracerLog::beginTransaction, this, &SqlLogDialog::saveSqlBeginTransactionToTraceLog, Qt::QueuedConnection);
        connect(sqlOperationTracerLog, &IqOrmSqlOperationTracerLog::commitTransaction, this, &SqlLogDialog::saveSqlCommitTransactionToTraceLog, Qt::QueuedConnection);
        connect(sqlOperationTracerLog, &IqOrmSqlOperationTracerLog::rollbackTransaction, this, &SqlLogDialog::saveSqlRollbackTransactionToTraceLog, Qt::QueuedConnection);
        connect(sqlOperationTracerLog, &IqOrmSqlOperationTracerLog::prepareQuery, this, &SqlLogDialog::saveSqlPrepereQueryToTraceLog, Qt::QueuedConnection);
        connect(sqlOperationTracerLog, &IqOrmSqlOperationTracerLog::execQuery, this, &SqlLogDialog::saveSqlExecQueryToTraceLog, Qt::QueuedConnection);
        connect(sqlOperationTracerLog, &IqOrmSqlOperationTracerLog::execPreparedQuery, this, &SqlLogDialog::saveSqlExecPreparedQueryToTraceLog, Qt::QueuedConnection);
#endif
    }
}

void SqlLogDialog::saveSqlBeginTransactionToTraceLog(bool ok, const QString &error)
{
    QString logRecord;

    if (ok)
        logRecord = tr("BEGIN TRANSACTION - Ok\n");
    else
        logRecord = tr("BEGIN TRANSACTION - Error\nError: %0\n")
                .arg(error);

    ui->sqlTraceLogTextEdit->append(logRecord.toHtmlEscaped());
}

void SqlLogDialog::saveSqlCommitTransactionToTraceLog(bool ok,
                                                            const QString &error)
{
    QString logRecord;

    if (ok)
        logRecord = tr("COMMIT TRANSACTION - Ok\n");
    else
        logRecord = tr("COMMIT TRANSACTION - Error\nError: %0\n")
                .arg(error);
    logRecord.append("------------------------------------------\n");

    ui->sqlTraceLogTextEdit->append(logRecord.toHtmlEscaped());
}

void SqlLogDialog::saveSqlRollbackTransactionToTraceLog(bool ok,
                                                              const QString &error)
{
    QString logRecord;

    if (ok)
        logRecord = tr("ROLLBACK TRANSACTION - Ok\n");
    else
        logRecord = tr("ROLLBACK TRANSACTION - Error\nError: %0\n")
                .arg(error);
    logRecord.append("------------------------------------------\n");

    ui->sqlTraceLogTextEdit->append(logRecord.toHtmlEscaped());
}

void SqlLogDialog::saveSqlPrepereQueryToTraceLog(const QString &query,
                                                       qint32 elapsed,
                                                       bool ok,
                                                       const QString &error)
{
    QString logRecord;

    if (ok)
        logRecord = tr("PREPARE %0 - Ok (%1 ms)\n")
                .arg(query)
                .arg(elapsed);
    else
        logRecord = tr("PREPARE %0 - Error (%1 ms)\nError: %2\n")
                .arg(query)
                .arg(elapsed)
                .arg(error);

    ui->sqlTraceLogTextEdit->append(logRecord.toHtmlEscaped());
}

void SqlLogDialog::saveSqlExecQueryToTraceLog(const QString &query,
                                                    qint32 elapsed,
                                                    bool ok,
                                                    const QString &error)
{
    QString logRecord;

    if (ok)
        logRecord = tr("EXEC %0 - Ok (%1 ms)\n")
                .arg(query)
                .arg(elapsed);
    else
        logRecord = tr("EXEC %0 - Error (%1 ms)\nError: %2\n")
                .arg(query)
                .arg(elapsed)
                .arg(error);

    ui->sqlTraceLogTextEdit->append(logRecord.toHtmlEscaped());
}

void SqlLogDialog::saveSqlExecPreparedQueryToTraceLog(const QVariantList &bindValues,
                                                            qint32 elapsed,
                                                            bool ok,
                                                            const QString &error)
{
    QString logRecord;

    QStringList bindStringList;
    foreach (const QVariant &value, bindValues) {
        bindStringList << QString("? - [%0] %1")
                          .arg(value.typeName())
                          .arg(value.toString());
    }

    if (ok)
        logRecord = tr("EXEC PREPARED%0 %1 - Ok (%2 ms)\n")
                .arg(bindStringList.isEmpty()?"":tr(" with"))
                .arg(bindStringList.join(", "))
                .arg(elapsed);
    else
        logRecord = tr("EXEC PREPARED%0 %1 - Error (%2 ms)\nError : %3\n")
                .arg(bindStringList.isEmpty()?"":tr(" with"))
                .arg(bindStringList.join(", "))
                .arg(elapsed)
                .arg(error);

    ui->sqlTraceLogTextEdit->append(logRecord.toHtmlEscaped());
}
