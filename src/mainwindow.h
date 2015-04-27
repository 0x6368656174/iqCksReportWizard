#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "sqllogdialog.h"
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
    QSplitter *m_splitter;
};

#endif // MAINWINDOW_H
