#include "filterswidget.h"
#include "ui_filterswidget.h"
#include <QSettings>
#include <QMessageBox>
#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QDebug>

FiltersWidget::FiltersWidget(QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::FiltersWidget),
    m_filtersModel(new FiltersModel(this)),
    m_findModel(Q_NULLPTR),
    m_mainWindow(Q_NULLPTR)
{
    ui->setupUi(this);
    ui->filtersTreeView->setModel(m_filtersModel);
}

FiltersWidget::~FiltersWidget()
{
    delete ui;
}

void FiltersWidget::setMainWindow(QMainWindow *mainWindow)
{
    m_mainWindow = mainWindow;
    ui->filtersTreeView->setMainWindow(mainWindow);
}

void FiltersWidget::find() const
{
    Q_CHECK_PTR(m_findModel);
    bool abort = false;
    QString error;
    IqOrmAbstractFilter *rootFilter = m_filtersModel->createFilter(&abort, &error);
    if (abort) {
                QMessageBox::warning(m_mainWindow,
                                     tr("Error on create filters"),
                                     error);
        return;
    }

    if (!rootFilter) {
        if(QMessageBox::question(m_mainWindow,
                                 tr("Search without filters?"),
                                 tr("Search without filters can take a very long time. Continue?")) != QMessageBox::Yes)
            return;
    }

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    m_findModel->setFilters(rootFilter);
    m_findModel->load();
    QApplication::restoreOverrideCursor();
}

void FiltersWidget::clearWithWarning() const
{
    clear(true);
}

void FiltersWidget::clear(bool warningDialogEnable) const
{
    if (m_filtersModel->rootFilter()->childCount() > 1
            && warningDialogEnable) {
        if (QMessageBox::question(m_mainWindow,
                                  tr("Clear all filters?"),
                                  tr("This operation clear root filter and remove all child filter. Continue?")) != QMessageBox::Yes)
            return;
    }
    m_filtersModel->setData(m_filtersModel->rootIndex(FiltersModel::TypeColumn), FilterItem::NotSetType, Qt::EditRole);
}

bool FiltersWidget::save(const QString &fileName) const
{
    return m_filtersModel->saveToFile(fileName);
}

bool FiltersWidget::load(const QString &fileName) const
{
    clear(false);

    bool result = m_filtersModel->loadFromFile(fileName);

    expandAll();

    return result;
}

void FiltersWidget::loadSettings()
{
    QSettings settigs;
    settigs.beginGroup("filtersConfig");
    QVariantList columnWidths = settigs.value("filterTableColumnWidths").toList();
    if (columnWidths.count() >= m_filtersModel->columnCount()) {
        for (int i = 0; i < m_filtersModel->columnCount(); ++i) {
            ui->filtersTreeView->setColumnWidth(i, columnWidths[i].toInt());
        }
    }

    settigs.endGroup();

    QString lastfilesFilePath = QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/" + "lastfilters.json";
    load(lastfilesFilePath);
}

void FiltersWidget::saveSettings() const
{
    QSettings settigs;
    settigs.beginGroup("filtersConfig");
    QVariantList columnWidths;
    for (int i = 0; i < m_filtersModel->columnCount(); ++i) {
        columnWidths << ui->filtersTreeView->columnWidth(i);
    }
    settigs.setValue("filterTableColumnWidths", columnWidths);

    settigs.endGroup();

    QDir dir;
    if (dir.mkpath(QStandardPaths::writableLocation(QStandardPaths::DataLocation))) {
        QString lastfilesFilePath = QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/" + "lastfilters.json";
        save(lastfilesFilePath);
        qDebug() << tr("Last filters saved to \"%0\".")
                    .arg(lastfilesFilePath);
    } else {
        qWarning() << tr("Enable to create \"%0\" dir.")
                      .arg(QStandardPaths::writableLocation(QStandardPaths::DataLocation));
    }
}

void FiltersWidget::collapseAll() const
{
    ui->filtersTreeView->collapseAll();
}

void FiltersWidget::expandAll() const
{
    ui->filtersTreeView->expandAll();
}


void FiltersWidget::setFindModel(IqOrmModel<ArchiveRecord> *findModel)
{
    m_findModel = findModel;
}

