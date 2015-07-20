#ifndef FILTERSWIDGET_H
#define FILTERSWIDGET_H

#include <QGroupBox>
#include "filtersmodel.h"
#include <QMainWindow>
#include <IqOrmAbstractFilter>
#include <IqOrmModel>
#include "archiverecord.h"

namespace Ui {
class FiltersWidget;
}

class FiltersWidget : public QGroupBox
{
    Q_OBJECT

public:
    explicit FiltersWidget(QWidget *parent = 0);
    ~FiltersWidget();

    void setMainWindow(QMainWindow *mainWindow);

    void setFindModel(IqOrmModel<ArchiveRecord> *findModel);

    void find() const;
    void clearWithWarning() const;

    bool save(const QString &fileName) const;
    bool load(const QString &fileName) const;
    
    void loadSettings();
    void saveSettings() const;

    void collapseAll() const;
    void expandAll() const;

private:
    void clear(bool warningDialogEnable) const;
    IqOrmAbstractFilter *createFilter(const FilterItem *filterItem,
                                      bool *abort) const;

private:
    Ui::FiltersWidget *ui;
    FiltersModel *m_filtersModel;
    IqOrmModel<ArchiveRecord> *m_findModel;
    QMainWindow *m_mainWindow;
    QHash<FilterItem::Properties, QString> m_propetriesNames;
};

#endif // FILTERSWIDGET_H
