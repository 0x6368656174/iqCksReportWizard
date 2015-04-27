#include "mainwindow.h"
#include <QApplication>
#include "logdialog.h"

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(context);
    switch (type) {
    case QtDebugMsg:
        LogDialog::addToLog(QObject::tr("Debug: %0")
                            .arg(msg));
        break;
    case QtWarningMsg:
        LogDialog::addToLog(QObject::tr("Warning: %0")
                            .arg(msg));
        break;
    case QtCriticalMsg:
        LogDialog::addToLog(QObject::tr("Critical: %0")
                            .arg(msg));
        break;
    case QtFatalMsg:
        LogDialog::addToLog(QObject::tr("Fatal: %0")
                            .arg(msg));
        abort();
    }
}


int main(int argc, char *argv[])
{
    qInstallMessageHandler(myMessageOutput);

    QApplication::setApplicationName("iqCksReportWizard");
    QApplication::setOrganizationName("itQuasar");
    QApplication::setApplicationVersion("1.0.0");

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
