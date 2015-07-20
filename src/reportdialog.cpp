#include "reportdialog.h"
#include "ui_reportdialog.h"
#include <QSettings>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QDebug>
#include <QStandardPaths>
#include <IqOrmModel>
#include <QMessageBox>
#include <QProgressDialog>
#include <QtConcurrent/QtConcurrentRun>
#include "archiverecord.h"
#include "filtersmodel.h"

ReportDialog::ReportDialog(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ReportDialog),
    m_mainWindow(Q_NULLPTR),
    m_progressDialog(new QProgressDialog(this))
{
    ui->setupUi(this);
    ui->startDateTimeDateTimeEdit->setDateTime(QDateTime::currentDateTime());
    ui->endDateTimeDateTimeEdit->setDateTime(QDateTime::currentDateTime());

    connect(ui->actionInsertRow, &QAction::triggered,
            this, &ReportDialog::insertRow);
    connect(ui->actionInsertColumn, &QAction::triggered,
            this, &ReportDialog::insertColumn);
    connect(ui->actionRemoveRow, &QAction::triggered,
            this, &ReportDialog::removeRow);
    connect(ui->actionRemoveColumn, &QAction::triggered,
            this, &ReportDialog::removeColumn);

    connect(ui->actionSave, &QAction::triggered,
            this, &ReportDialog::showSaveDialog);
    connect(ui->actionOpen, &QAction::triggered,
            this, &ReportDialog::showOpenDialog);

    connect(ui->actionChoiseFilter, &QAction::triggered,
            this, &ReportDialog::choiseFilter);

    connect(ui->actionCreate, &QAction::triggered,
            this, &ReportDialog::createReport);

    ui->reportTableWidget->addAction(ui->actionInsertRow);
    ui->reportTableWidget->addAction(ui->actionInsertColumn);
    ui->reportTableWidget->addAction(ui->actionRemoveRow);
    ui->reportTableWidget->addAction(ui->actionRemoveColumn);
    ui->reportTableWidget->addAction(ui->actionChoiseFilter);
    ui->reportTableWidget->setContextMenuPolicy(Qt::ActionsContextMenu);

    setWindowModality(Qt::ApplicationModal);
    loadSettings();
    QString lastReportFilePath = QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/" + "lastreport.json";
    load(lastReportFilePath);

    connect(ui->reportTableWidget, &QTableWidget::currentCellChanged,
            this, [this](int currentRow, int currentColumn, int , int ){
        if (currentRow != -1)
            ui->actionRemoveRow->setEnabled(true);
        else
            ui->actionRemoveRow->setEnabled(false);
        if (currentColumn != -1)
            ui->actionRemoveColumn->setEnabled(true);
        else
            ui->actionRemoveColumn->setEnabled(false);
        if (currentRow != -1 && currentColumn != -1)
            ui->actionChoiseFilter->setEnabled(true);
        else
            ui->actionChoiseFilter->setEnabled(false);

    });

    m_progressDialog->setLabelText(tr("Report created..."));
    m_progressDialog->setCancelButtonText(tr("Abort"));
    m_progressDialog->setMinimum(0);
    m_progressDialog->setWindowModality(Qt::WindowModal);

    connect(&m_progressFutureWatcher, &QFutureWatcher<QString>::finished,
            this, [this](){
        QString result = m_progressFutureWatcher.result();
        if (!result.isEmpty())
            QMessageBox::warning(this, tr("Error on create report"), result);
    });
}

ReportDialog::~ReportDialog()
{
    delete ui;
}

void ReportDialog::show()
{
    QMainWindow::show();
    loadSettings();
}

void ReportDialog::closeEvent(QCloseEvent *event)
{
    QSettings settings;

    settings.beginGroup("reportDialogConfig");
    QVariantList columnWidths;
    for (int i = 0; i < ui->reportTableWidget->columnCount(); ++i) {
        columnWidths << ui->reportTableWidget->columnWidth(i);
    }
    settings.setValue("reportTableColumnWidths", columnWidths);

    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
    settings.endGroup();

    QString lastReportFilePath = QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/" + "lastreport.json";
    save(lastReportFilePath);
    QMainWindow::closeEvent(event);
}

void ReportDialog::insertRow()
{
    if (ui->reportTableWidget->currentRow() == -1)
        ui->reportTableWidget->insertRow(ui->reportTableWidget->rowCount());
    else
        ui->reportTableWidget->insertRow(ui->reportTableWidget->currentRow() + 1);
}

void ReportDialog::insertColumn()
{
    if (ui->reportTableWidget->currentColumn() == -1)
        ui->reportTableWidget->insertColumn(ui->reportTableWidget->columnCount());
    else
        ui->reportTableWidget->insertColumn(ui->reportTableWidget->currentColumn() + 1);
}

void ReportDialog::removeRow()
{
    if (ui->reportTableWidget->rowCount() == 0)
        return;

    if (ui->reportTableWidget->currentRow() == -1)
        ui->reportTableWidget->removeRow(ui->reportTableWidget->rowCount() - 1);
    else
        ui->reportTableWidget->removeRow(ui->reportTableWidget->currentRow());
}

void ReportDialog::removeColumn()
{
    if (ui->reportTableWidget->columnCount() == 0)
        return;

    if (ui->reportTableWidget->currentColumn() == -1)
        ui->reportTableWidget->removeColumn(ui->reportTableWidget->columnCount() - 1);
    else
        ui->reportTableWidget->removeColumn(ui->reportTableWidget->currentColumn());
}

void ReportDialog::loadSettings()
{
    QSettings settings;
    settings.beginGroup("reportDialogConfig");
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());

    QVariantList columnWidths = settings.value("reportTableColumnWidths").toList();
    if (columnWidths.count() >= ui->reportTableWidget->columnCount()) {
        for (int i = 0; i < ui->reportTableWidget->columnCount(); ++i) {
            ui->reportTableWidget->setColumnWidth(i, columnWidths[i].toInt());
        }
    }
    settings.endGroup();
}

void ReportDialog::showSaveDialog()
{
    QSettings settins;
    settins.beginGroup("reportDialogConfig");
    QString openDir = settins.value("lastOpenReportTemplateDir").toString();
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save report template"), openDir,
                                                    tr("JSON Report templates (*.json)"));
    if (fileName.isEmpty())
        return;
    QFileInfo fileInfo (fileName);
    settins.setValue("lastOpenReportTemplateDir", fileInfo.path());
    settins.endGroup();

    if (!fileName.isEmpty())
        save(fileName);
}

void ReportDialog::showOpenDialog()
{
    QSettings settins;
    settins.beginGroup("reportDialogConfig");
    QString openDir = settins.value("lastOpenReportTemplateDir").toString();
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open report template"), openDir,
                                                    tr("JSON Report templates (*.json)"));
    if (fileName.isEmpty())
        return;
    QFileInfo fileInfo (fileName);
    settins.setValue("lastOpenReportTemplateDir", fileInfo.path());
    settins.endGroup();

    if (!fileName.isEmpty())
        load(fileName);
}

bool ReportDialog::save(const QString &fileName) const
{
    if (fileName.isEmpty())
        return false;

    QFile file (fileName);
    if (!file.open(QFile::WriteOnly | QFile::Truncate)) {
        qWarning() << tr("Error on open file \"%0\" for write.")
                      .arg(fileName);
        return false;
    }

    QJsonObject rootObject;

    QJsonObject templatesObject;

    QJsonValue startDateTimeVaule (ui->startDateTimeDateTimeEdit->dateTime().toString(Qt::ISODate));
    templatesObject.insert("startDateTime", startDateTimeVaule);

    QJsonValue endDateTimeVaule (ui->endDateTimeDateTimeEdit->dateTime().toString(Qt::ISODate));
    templatesObject.insert("endDateTime", endDateTimeVaule);

    rootObject.insert("templates", templatesObject);

    QJsonArray reportTableArray;

    for (int i = 0; i < ui->reportTableWidget->rowCount(); ++i) {
        QJsonArray rowArray;
        for (int j = 0; j < ui->reportTableWidget->columnCount(); ++j) {
            QTableWidgetItem *item = ui->reportTableWidget->item(i, j);
            QJsonValue row ("");
            if (item)
                row = QJsonValue(item->text());
            rowArray.append(row);
        }
        reportTableArray.append(rowArray);
    }

    rootObject.insert("reportTable", reportTableArray);

    QJsonDocument jsonDoc;
    jsonDoc.setObject(rootObject);

    file.write(jsonDoc.toJson());

    return true;
}

bool ReportDialog::load(const QString &fileName)
{
    ui->reportTableWidget->clear();

    QFile file (fileName);
    if (!file.open(QFile::ReadOnly)) {
        qWarning() << tr("Error on open file \"%0\" for read.")
                      .arg(fileName);
        return false;
    }

    QJsonParseError parseError;
    QJsonDocument jsonDoc;
    jsonDoc = QJsonDocument::fromJson(file.readAll(), &parseError);
    if (jsonDoc.isNull()) {
        qWarning() << tr("Error on load json from \"%0\". Error: %1.")
                      .arg(fileName)
                      .arg(parseError.errorString());
        return false;
    }

    if (!jsonDoc.isObject()) {
        qWarning() << tr("Invalid json in file \"%0\".")
                      .arg(fileName);
        return false;
    }

    QJsonObject rootObject = jsonDoc.object();
    if (rootObject.isEmpty())
        return true;

    QJsonObject templatesObject = rootObject.value("templates").toObject();
    if (templatesObject.contains("startDateTime")) {
        QJsonValue startDateTime = templatesObject.value("startDateTime");
        ui->startDateTimeDateTimeEdit->setDateTime(QDateTime::fromString(startDateTime.toString(), Qt::ISODate));
    }
    if (templatesObject.contains("endDateTime")) {
        QJsonValue endDateTime = templatesObject.value("endDateTime");
        ui->endDateTimeDateTimeEdit->setDateTime(QDateTime::fromString(endDateTime.toString(), Qt::ISODate));
    }

    QJsonArray reportTableArray = rootObject.value("reportTable").toArray();
    for (int i = 0; i < reportTableArray.count(); ++i) {
        ui->reportTableWidget->insertRow(i);
        QJsonArray row = reportTableArray.at(i).toArray();
        for (int j = 0; j < row.count(); ++j) {
            if (i == 0)
                ui->reportTableWidget->insertColumn(j);
            QTableWidgetItem *item = new QTableWidgetItem(row.at(j).toString());
            ui->reportTableWidget->setItem(i, j, item);
        }
    }

    return true;
}

void ReportDialog::choiseFilter()
{
    if (ui->reportTableWidget->currentRow() == -1)
        return;

    QSettings settins;
    QString openDir = settins.value("lastOpenFiltersDir").toString();
    QString fileName = QFileDialog::getOpenFileName(this, tr("Choise filter"), openDir,
                                                    tr("JSON Filters (*.json)"));
    if (fileName.isEmpty())
        return;
    QFileInfo fileInfo (fileName);
    settins.setValue("lastOpenFiltersDir", fileInfo.path());

    if (!ui->reportTableWidget->currentItem())
        ui->reportTableWidget->setItem(ui->reportTableWidget->currentRow(),
                                       ui->reportTableWidget->currentColumn(),
                                       new QTableWidgetItem(""));
    ui->reportTableWidget->currentItem()->setText(QString("filtercount://%0")
                                                  .arg(fileName));
}

void ReportDialog::createReport()
{
    QSettings settins;
    settins.beginGroup("reportDialogConfig");
    QString openDir = settins.value("lastSaveReportDir").toString();
    QString fileName = QFileDialog::getSaveFileName(this, tr("Create report"), openDir,
                                                    tr("CSV Reports (*.csv)"));
    if (fileName.isEmpty())
        return;
    QFileInfo fileInfo (fileName);
    settins.setValue("lastSaveReportDir", fileInfo.path());
    settins.endGroup();

    int numFilters = 0;
    for (int row = 0; row < ui->reportTableWidget->rowCount(); ++row) {
        for (int column = 0; column < ui->reportTableWidget->columnCount(); ++column) {
            QString itemText;
            if (ui->reportTableWidget->item(row, column))
                itemText = ui->reportTableWidget->item(row, column)->text();
            if (itemText.trimmed().startsWith("filtercount://")) {
                numFilters++;
            }
        }
    }

    m_progressDialog->reset();
    m_progressDialog->setMaximum(numFilters);
    m_progressDialog->show();

    QFuture<QString> future = QtConcurrent::run(this, &ReportDialog::processCreateReport, fileName, numFilters);
    m_progressFutureWatcher.setFuture(future);
}

QString ReportDialog::processCreateReport(const QString fileName, const int numFilters)
{
    auto escapeText = [](const QString &text) {
        QString result = text;
        result.replace(QChar('"'), QString("\"\""));
        return '"' + result + '"';
    };

    QFile file (fileName);
    if (!file.open(QFile::WriteOnly | QFile::Truncate)) {
        m_progressDialog->hide();
        return tr("Error on open file \"%0\" for write.")
                             .arg(fileName);
    }


    int filterCompleted = 0;
    QStringList columnList;
    for (int row = 0; row < ui->reportTableWidget->rowCount(); ++row) {
        QStringList rowList;
        for (int column = 0; column < ui->reportTableWidget->columnCount(); ++column) {
            if (m_progressDialog->wasCanceled()) {
                file.close();
                return "";
            }
            QMetaObject::invokeMethod(m_progressDialog, "setValue", Q_ARG(int, filterCompleted));

            QString itemText;
            if (ui->reportTableWidget->item(row, column))
                itemText = ui->reportTableWidget->item(row, column)->text();

            if (!itemText.trimmed().startsWith("filtercount://")) {
                rowList << escapeText(itemText);
                continue;
            }

            FiltersModel filterModel;
            itemText = itemText.trimmed().remove("filtercount://");

            if (!filterModel.loadFromFile(itemText)) {
                file.close();
                QMetaObject::invokeMethod(m_progressDialog, "reset");
                return tr("Error on load filters from \"%0\".")
                        .arg(itemText);
            }

            replaceFilterTemplate(filterModel.rootFilter());

            IqOrmModel<ArchiveRecord> findModel;
            bool abort = false;
            QString error;
            findModel.setFilters(filterModel.createFilter(&abort, &error));
            if (abort) {
                file.close();
                QMetaObject::invokeMethod(m_progressDialog, "reset");
                return tr("Error on create filters from \"%0\". Error: %1.")
                        .arg(itemText)
                        .arg(error);
            }

            int count = findModel.loadCount();

            rowList << escapeText(QString::number(count));

            filterCompleted++;
        }
        columnList << rowList.join(';');
    }

    QMetaObject::invokeMethod(m_progressDialog, "reset");

    QString crlf;
    crlf.append(QChar(QChar::CarriageReturn));
    crlf.append(QChar(QChar::LineFeed));

    QString result = columnList.join(crlf);

    file.write(result.toLocal8Bit());
    file.close();
    return "";
}

void ReportDialog::replaceFilterTemplate(FilterItem *item)
{
    Q_CHECK_PTR(item);

    if (item->type() == FilterItem::NotSetType)
        return;

    if (item->type() == FilterItem::Condition) {
        switch (item->reportTemplate()) {
        case FilterItem::StartDateTime:
            item->setValue(ui->startDateTimeDateTimeEdit->dateTime());
            break;
        case FilterItem::EndDateTime:
            item->setValue(ui->endDateTimeDateTimeEdit->dateTime());
            break;
        default:
            break;
        }

        return;
    }

    foreach (FilterItem *child, item->childFilters()) {
        replaceFilterTemplate(child);
    }
}

QMainWindow *ReportDialog::mainWindow() const
{
    return m_mainWindow;
}

void ReportDialog::setMainWindow(QMainWindow *mainWindow)
{
    m_mainWindow = mainWindow;
}

