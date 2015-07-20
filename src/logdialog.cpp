#include "logdialog.h"
#include "ui_logdialog.h"
#include <QDateTime>

LogDialog *LogDialog::m_instanse = Q_NULLPTR;
LogDialog::LogDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LogDialog),
    m_syntaxHighliter(new LogSyntaxHighliter(this))
{
    ui->setupUi(this);
    m_instanse = this;
    m_syntaxHighliter->setDocument(ui->logTextEdit->document());
}

LogDialog::~LogDialog()
{
    delete ui;
}

void LogDialog::addToLog(const QString &message)
{
    if (m_instanse) {
        QString messageToLog = QString("<%0> %1")
                .arg(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss"))
                .arg(message);
        QMetaObject::invokeMethod(m_instanse, "sheldueAddToLog", Qt::QueuedConnection,
                                  Q_ARG(QString, messageToLog));
    }

}

void LogDialog::sheldueAddToLog(const QString &message)
{
    ui->logTextEdit->append(message.toHtmlEscaped());
}
