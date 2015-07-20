#ifndef REPORTDIALOG_H
#define REPORTDIALOG_H

#include <QMainWindow>
#include <QProgressDialog>
#include <QFutureWatcher>
#include "filteritem.h"

namespace Ui {
class ReportDialog;
}

class ReportDialog : public QMainWindow
{
    Q_OBJECT

public:
    explicit ReportDialog(QWidget *parent = 0);
    ~ReportDialog();

    void show();

    QMainWindow *mainWindow() const;
    void setMainWindow(QMainWindow *mainWindow);

protected:
    virtual void closeEvent(QCloseEvent *event);

private:
    void insertRow();
    void insertColumn();
    void removeRow();
    void removeColumn();
    void loadSettings();
    void showSaveDialog();
    void showOpenDialog();
    bool save(const QString &fileName) const;
    bool load(const QString &fileName);
    void choiseFilter();
    void createReport();
    QString processCreateReport(const QString fileName, const int numFilters);
    void replaceFilterTemplate(FilterItem *item);

private:
    Ui::ReportDialog *ui;
    QMainWindow *m_mainWindow;
    QProgressDialog *m_progressDialog;
    QFutureWatcher<QString> m_progressFutureWatcher;
};

#endif // REPORTDIALOG_H
