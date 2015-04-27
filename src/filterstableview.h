#ifndef FILTERSTABLEVIEW_H
#define FILTERSTABLEVIEW_H

#include <QTreeView>
#include <QMainWindow>

class FiltersTableView : public QTreeView
{
public:
    explicit FiltersTableView(QWidget *parent = Q_NULLPTR);
    ~FiltersTableView();

    void setMainWindow(QMainWindow *mainWindow);
};

#endif // FILTERSTABLEVIEW_H
