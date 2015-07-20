#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "sqllogdialog.h"
#include "logdialog.h"
#include "reportdialog.h"
#include <QSplitter>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    virtual void closeEvent(QCloseEvent *event);

private:
    void showConfigDialog();
    void loadSettings();
    void showAbout();
    void openFilters();
    void saveFilters();

private:
    Ui::MainWindow *ui;
    SqlLogDialog *m_sqlLogDialog;
    LogDialog *m_logDialog;
    QSplitter *m_splitter;
    ReportDialog *m_reportDialog;
};

#endif // MAINWINDOW_H
