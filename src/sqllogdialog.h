#ifndef SQLLOGDIALOG_H
#define SQLLOGDIALOG_H

#include <QDialog>
#include "sqltracelogsyntaxhighliter.h"
#include <IqOrmSqlOperationTracerLog>

namespace Ui {
class SqlLogDialog;
}

class SqlLogDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SqlLogDialog(QWidget *parent = 0);
    ~SqlLogDialog();

    void setSqlOperationTracerLog(IqOrmSqlOperationTracerLog *sqlOperationTracerLog);

private slots:
    void saveSqlBeginTransactionToTraceLog(bool ok,
                                            const QString &error);

    void saveSqlCommitTransactionToTraceLog(bool ok,
                                            const QString &error);

    void saveSqlRollbackTransactionToTraceLog(bool ok,
                                              const QString &error);

    void saveSqlPrepereQueryToTraceLog(const QString &query,
                                       qint32 elapsed,
                                       bool ok,
                                       const QString &error);

    void saveSqlExecQueryToTraceLog(const QString &query,
                                    qint32 elapsed,
                                    bool ok,
                                    const QString &error);

    void saveSqlExecPreparedQueryToTraceLog(const QVariantList &bindValues,
                                            qint32 elapsed,
                                            bool ok,
                                            const QString &error);

private:
    Ui::SqlLogDialog *ui;
    SqlTraceLogSyntaxHighliter *m_syntaxHighliter;
};

#endif // SQLLOGDIALOG_H
