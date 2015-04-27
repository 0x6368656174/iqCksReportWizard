#ifndef LOGDIALOG_H
#define LOGDIALOG_H

#include <QDialog>
#include "logsynaxhigliter.h"

namespace Ui {
class LogDialog;
}

class LogDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LogDialog(QWidget *parent = 0);
    ~LogDialog();

    static void addToLog(const QString &message);

private:
    Q_INVOKABLE void sheldueAddToLog(const QString &message);

    void changeCodepage();

private:
    Ui::LogDialog *ui;
    static LogDialog *m_instanse;
    QByteArray m_sourceLog;
    QString m_codePage;
    LogSyntaxHighliter *m_syntaxHighliter;
};

#endif // LOGDIALOG_H
