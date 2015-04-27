#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication::setApplicationName("iqCksReportWizard");
    QApplication::setOrganizationName("itQuasar");
    QApplication::setApplicationVersion("1.0.0");

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
