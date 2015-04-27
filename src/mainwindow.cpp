#define DATABASENAME "dbconnections"

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTreeView>
#include "configdialog.h"
#include <QSqlDatabase>
#include <QSettings>
#include <IqOrmCore>
#include <IqOrmSqlDataSource>
#include <QMessageBox>
#include <QFileDialog>

#include "archiverecord.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_sqlLogDialog(new SqlLogDialog(this)),
    m_logDialog(new LogDialog(this)),
    m_splitter(new QSplitter(this))
{
    ui->setupUi(this);
    QSettings settings;

    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL", DATABASENAME);
    settings.beginGroup("databaseConfig");
    db.setHostName(settings.value("host", "localhost").toString());
    db.setPort(settings.value("port", 5432).toInt());
    db.setUserName(settings.value("user", "postgres").toString());
    db.setPassword(settings.value("password").toString());
    db.setDatabaseName(settings.value("databaseName", "cks").toString());
    settings.endGroup();

    IqOrmSqlDataSource *ds = new IqOrmSqlDataSource(this);
    ds->setDatabase(db);
    IqOrmCore::setDataSource(ds);
    m_sqlLogDialog->setSqlOperationTracerLog(ds->tracerLog());

    ui->filtersWidget->setMainWindow(this);
    ui->filtersWidget->setFindModel(ui->resultWidget->findModel());

    connect(ui->actionSettings, &QAction::triggered,
            this, &MainWindow::showConfigDialog);

    connect(ui->actionLog, &QAction::triggered,
            m_logDialog, &LogDialog::show);
    connect(ui->actionSQL_Log, &QAction::triggered,
            m_sqlLogDialog, &SqlLogDialog::show);

    connect(ui->actionFind, &QAction::triggered,
            ui->filtersWidget, &FiltersWidget::find);
    connect(ui->actionClear, &QAction::triggered,
            ui->filtersWidget, &FiltersWidget::clearWithWarning);
    connect(ui->actionOpen, &QAction::triggered,
            this, &MainWindow::openFilters);
    connect(ui->actionSave, &QAction::triggered,
            this, &MainWindow::saveFilters);

    connect(ui->actionAboutQt, &QAction::triggered,
            this, [this](){QMessageBox::aboutQt(this);});

    connect(ui->actionAbout, &QAction::triggered,
            this, &MainWindow::showAbout);

    m_splitter->setOrientation(Qt::Vertical);
    m_splitter->addWidget(ui->filtersWidget);
    m_splitter->addWidget(ui->resultWidget);
    ui->mainLayout->insertWidget(0, m_splitter);

    loadSettings();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showConfigDialog()
{
    ConfigDialog *configDialog = new ConfigDialog(this);
    connect(configDialog, &ConfigDialog::finished,
            configDialog, &ConfigDialog::deleteLater);
    if(configDialog->exec()) {
        QSqlDatabase db = QSqlDatabase::database(DATABASENAME);
        db.close();

        QSettings settings;
        settings.beginGroup("databaseConfig");
        db.setHostName(settings.value("host", "localhost").toString());
        db.setPort(settings.value("port", 5432).toInt());
        db.setUserName(settings.value("user", "postgres").toString());
        db.setPassword(settings.value("password").toString());
        db.setDatabaseName(settings.value("databaseName", "cks").toString());
        settings.endGroup();
    }
}

void MainWindow::loadSettings()
{
    QSettings settings;
    settings.beginGroup("mainWidowConfig");
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());
    QList<int> splitterSizes;
    foreach (const QVariant &size, settings.value("splitterSizes").toList()) {
        splitterSizes << size.toInt();
    }
    m_splitter->setSizes(splitterSizes);
    settings.endGroup();
    ui->filtersWidget->loadSettings();
    ui->resultWidget->loadSettings();
}

void MainWindow::showAbout()
{
    QMessageBox::about(this, tr("IqCksReportWizard about"), tr("This is very good app."));
}

void MainWindow::openFilters()
{
    QSettings settins;
    QString openDir = settins.value("lastOpenFiltersDir").toString();
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open filtes"), openDir, "JSON Filters (*.json)");
    QFileInfo fileInfo (fileName);
    settins.setValue("lastOpenFiltersDir", fileInfo.path());

    if (!fileName.isEmpty())
        ui->filtersWidget->load(fileName);
}

void MainWindow::saveFilters()
{
    QSettings settins;
    QString openDir = settins.value("lastOpenFiltersDir").toString();
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save filtes"), openDir, "JSON Filters (*.json)");
    QFileInfo fileInfo (fileName);
    settins.setValue("lastOpenFiltersDir", fileInfo.path());

    if (!fileName.isEmpty())
        ui->filtersWidget->save(fileName);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings;
    settings.beginGroup("mainWidowConfig");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
    QVariantList splitterSizes;
    foreach (int size, m_splitter->sizes()) {
        splitterSizes << size;
    }
    settings.setValue("splitterSizes", splitterSizes);
    settings.endGroup();

    ui->filtersWidget->saveSettings();
    ui->resultWidget->saveSettings();
    QMainWindow::closeEvent(event);
}
