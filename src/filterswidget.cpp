#include "filterswidget.h"
#include "ui_filterswidget.h"
#include <IqOrmAndGroupFilter>
#include <IqOrmOrGroupFilter>
#include <IqOrmFilter>
#include <QSettings>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
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

    m_propetriesNames[FilterItem::ChannelName] = "channelName";
    m_propetriesNames[FilterItem::ChannelNumber] = "channelNumber";
    m_propetriesNames[FilterItem::HeaderInfo] = "headerInfo";
    m_propetriesNames[FilterItem::Priority] = "priority";
    m_propetriesNames[FilterItem::Addresses] = "addresses";
    m_propetriesNames[FilterItem::Cc] = "cc";
    m_propetriesNames[FilterItem::SenderTime] = "senderTime";
    m_propetriesNames[FilterItem::Sender] = "sender";
    m_propetriesNames[FilterItem::SenderInfo] = "senderInfo";
    m_propetriesNames[FilterItem::Text] = "text";
    m_propetriesNames[FilterItem::DateTime] = "dateTime";
    m_propetriesNames[FilterItem::Direction] = "direction";
    m_propetriesNames[FilterItem::ChannelId] = "channelId";
    m_propetriesNames[FilterItem::Journal] = "journal";
    m_propetriesNames[FilterItem::SerialNumber] = "serialNumber";
    m_propetriesNames[FilterItem::Svc] = "svc";
    m_propetriesNames[FilterItem::RouteId] = "routeId";
    m_propetriesNames[FilterItem::MessageType] = "msgType";
    m_propetriesNames[FilterItem::NotSetPorperty] = "";
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
    IqOrmAbstractFilter *rootFilter = createFilter(m_filtersModel->rootFilter());
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
    QFile file (fileName);
    if (!file.open(QFile::WriteOnly | QFile::Truncate)) {
        qWarning() << tr("Error on open file \"%0\" for write.")
                      .arg(fileName);
        return false;
    }

    QJsonObject rootObject = createFilterJson(m_filtersModel->rootFilter());
    QJsonDocument jsonDoc;
    jsonDoc.setObject(rootObject);


    file.write(jsonDoc.toJson());
    return true;
}

bool FiltersWidget::load(const QString &fileName) const
{
    clear(false);

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

    createItemFromJson(rootObject, m_filtersModel->invisibleRootItemIndex(0));

    expandAll();

    return true;
}

IqOrmAbstractFilter *FiltersWidget::createFilter(const FilterItem *filterItem) const
{
    Q_CHECK_PTR(filterItem);
    IqOrmAbstractFilter *filter = Q_NULLPTR;

    switch (filterItem->type()) {
    case FilterItem::NotSetType:
        break;
    case FilterItem::Condition: {
        if (filterItem->property() == FilterItem::NotSetPorperty)
            break;
        if (filterItem->operation() == FilterItem::NotSetOperation)
            break;

        IqOrmFilter *resultFilter = new IqOrmFilter();

        switch (filterItem->property()) {
        case FilterItem::NotSetPorperty:
            break;
        default:
            Q_ASSERT(m_propetriesNames.contains(filterItem->property()));
            resultFilter->setProperty(m_propetriesNames[filterItem->property()]);
            break;
        }

        switch (filterItem->operation()) {
        case FilterItem::NotSetOperation:
            break;
        case FilterItem::Equals:
            resultFilter->setCondition(IqOrmFilter::Equals);
            break;
        case FilterItem::StartsWith:
            resultFilter->setCondition(IqOrmFilter::StartsWith);
            break;
        case FilterItem::EndsWith:
            resultFilter->setCondition(IqOrmFilter::EndsWith);
            break;
        case FilterItem::Contains:
            resultFilter->setCondition(IqOrmFilter::Contains);
            break;
        case FilterItem::Greater:
            resultFilter->setCondition(IqOrmFilter::GreaterThan);
            break;
        case FilterItem::GreaterOrEquals:
            resultFilter->setCondition(IqOrmFilter::GreaterOrEquals);
            break;
        case FilterItem::Less:
            resultFilter->setCondition(IqOrmFilter::LessThan);
            break;
        case FilterItem::LessOrEquals:
            resultFilter->setCondition(IqOrmFilter::LessOrEquals);
            break;
        case FilterItem::Overlap:
            resultFilter->setCondition(IqOrmFilter::Overlap);
            break;
        }

        resultFilter->setValue(filterItem->value());
        resultFilter->setCaseSensitivity(filterItem->caseSensitivity());
        resultFilter->setInverted(filterItem->inverted());
        filter = resultFilter;
        break;
    }
    case FilterItem::AndGroup: {
        IqOrmAndGroupFilter *resultFilter = new IqOrmAndGroupFilter();

        foreach (const FilterItem *childFilter, filterItem->childFilters()) {
            IqOrmAbstractFilter *childResultFilter = createFilter(childFilter);
            if (childResultFilter)
                resultFilter->add(childResultFilter);
        }

        if (resultFilter->count() == 0)
            resultFilter->deleteLater();
        else
            filter = resultFilter;
        break;
    }
    case FilterItem::OrGroup: {
        IqOrmOrGroupFilter *resultFilter = new IqOrmOrGroupFilter();

        foreach (const FilterItem *childFilter, filterItem->childFilters()) {
            IqOrmAbstractFilter *childResultFilter = createFilter(childFilter);
            if (childResultFilter)
                resultFilter->add(childResultFilter);
        }

        if (resultFilter->count() == 0)
            resultFilter->deleteLater();
        else
            filter = resultFilter;
        break;
    }
    }

    return filter;
}

QJsonObject FiltersWidget::createFilterJson(const FilterItem *filterItem) const
{
    Q_CHECK_PTR(filterItem);
    switch (filterItem->type()) {
    case FilterItem::NotSetType:
        break;
    case FilterItem::Condition: {
        if (filterItem->property() == FilterItem::NotSetPorperty)
            return QJsonObject();
        if (filterItem->operation() == FilterItem::NotSetOperation)
            return QJsonObject();

        QJsonObject resultObject;
        resultObject.insert("type", "condition");

        QString propertyName;
        switch (filterItem->property()) {
        case FilterItem::NotSetPorperty:
            break;
        default:
            Q_ASSERT(m_propetriesNames.contains(filterItem->property()));
            propertyName = m_propetriesNames[filterItem->property()];
        }
        resultObject.insert("property", propertyName);

        QString operation;
        switch (filterItem->operation()) {
        case FilterItem::NotSetOperation:
            break;
        case FilterItem::Equals:
            operation = "equals";
            break;
        case FilterItem::StartsWith:
            operation = "startsWith";
            break;
        case FilterItem::EndsWith:
            operation = "endsWith";
            break;
        case FilterItem::Contains:
            operation = "contains";
            break;
        case FilterItem::Greater:
            operation = "greaterThan";
            break;
        case FilterItem::GreaterOrEquals:
            operation = "greaterOrEquals";
            break;
        case FilterItem::Less:
            operation = "lessThan";
            break;
        case FilterItem::LessOrEquals:
            operation = "lessOrEquals";
            break;
        case FilterItem::Overlap:
            operation = "overlap";
            break;
        }
        resultObject.insert("operation", operation);

        if (filterItem->value().canConvert<double>()
                && filterItem->value().type() != QVariant::String
                && filterItem->value().type() != QVariant::ByteArray)
            resultObject.insert("value", filterItem->value().toDouble());
        else if (filterItem->value().type() == QVariant::Bool)
            resultObject.insert("value", filterItem->value().toBool());
        else if (filterItem->value().type() == QVariant::StringList) {
            QJsonArray valueArray;
            foreach (const QString &string, filterItem->value().toStringList()) {
                valueArray.append(QJsonValue(string));
            }
            resultObject.insert("value", valueArray);
        } else if (filterItem->value().canConvert<QString>())
            resultObject.insert("value", filterItem->value().toString());
        else
            qWarning() << tr("Error to save filter value to Json.");

        resultObject.insert("caseSensitivity", filterItem->caseSensitivity() == Qt::CaseSensitive);
        resultObject.insert("inverted", filterItem->inverted());

        return resultObject;

        break;
    }
    case FilterItem::AndGroup: {
        QJsonObject resultObject;
        resultObject.insert("type", "and");

        QJsonArray resultArray;

        foreach (const FilterItem *childFilter, filterItem->childFilters()) {
            QJsonObject childResultObject = createFilterJson(childFilter);
            if (!childResultObject.isEmpty())
                resultArray.append(childResultObject);
        }
        resultObject.insert("childs", resultArray);
        return resultObject;

        break;
    }
    case FilterItem::OrGroup: {
        QJsonObject resultObject;
        resultObject.insert("type", "or");

        QJsonArray resultArray;

        foreach (const FilterItem *childFilter, filterItem->childFilters()) {
            QJsonObject childResultObject = createFilterJson(childFilter);
            if (!childResultObject.isEmpty())
                resultArray.append(childResultObject);
        }
        resultObject.insert("childs", resultArray);
        return resultObject;

        break;
    }
    }

    return QJsonObject();
}

bool FiltersWidget::createItemFromJson(const QJsonObject &filterObject, const QModelIndex &parentIndex) const
{
    int row = m_filtersModel->rowCount(parentIndex) - 1;
    Q_ASSERT(row > -1);
    QString itemType = filterObject.value("type").toString();

    if (itemType.compare("condition", Qt::CaseInsensitive) == 0) {
        QString property = filterObject.value("property").toString();
        FilterItem::Properties itemProperty = FilterItem::NotSetPorperty;

        QHashIterator<FilterItem::Properties, QString> properitesNamesI(m_propetriesNames);
        bool propertyFound = false;
        while (properitesNamesI.hasNext()) {
            properitesNamesI.next();
            if (property.compare(properitesNamesI.value(), Qt::CaseInsensitive) == 0) {
                itemProperty = properitesNamesI.key();
                propertyFound = true;
                break;
            }
        }

        if (!propertyFound)
            qWarning() << tr("Unknown property \"%0\".")
                          .arg(property);

        if (itemProperty == FilterItem::NotSetPorperty)
            return false;

        QString operation = filterObject.value("operation").toString();
        FilterItem::Operation itemOperation = FilterItem::NotSetOperation;

        if (operation.compare("equals", Qt::CaseInsensitive) == 0)
            itemOperation = FilterItem::Equals;
        else if (operation.compare("startsWith", Qt::CaseInsensitive) == 0)
            itemOperation = FilterItem::StartsWith;
        else if (operation.compare("endsWith", Qt::CaseInsensitive) == 0)
            itemOperation = FilterItem::EndsWith;
        else if (operation.compare("contains", Qt::CaseInsensitive) == 0)
            itemOperation = FilterItem::Contains;
        else if (operation.compare("greaterThan", Qt::CaseInsensitive) == 0)
            itemOperation = FilterItem::Greater;
        else if (operation.compare("greaterOrEquals", Qt::CaseInsensitive) == 0)
            itemOperation = FilterItem::GreaterOrEquals;
        else if (operation.compare("lessThan", Qt::CaseInsensitive) == 0)
            itemOperation = FilterItem::Less;
        else if (operation.compare("lessOrEquals", Qt::CaseInsensitive) == 0)
            itemOperation = FilterItem::LessOrEquals;
        else if (operation.compare("overlap", Qt::CaseInsensitive) == 0)
            itemOperation = FilterItem::Overlap;
        else
            qWarning() << tr("Unknown operation \"%0\".")
                          .arg(operation);

        if (itemOperation == FilterItem::NotSetOperation)
            return false;


        QJsonValue value = filterObject.value("value");
        QVariant itemValue;
        if (value.isBool())
            itemValue = value.toBool();
        else if (value.isDouble())
            itemValue = value.toDouble();
        else if (value.isString())
            itemValue = value.toString();
        else if (value.isArray()) {
            QStringList valueStringList;
            QJsonArray valueArray = value.toArray();
            for (int i = 0; i < valueArray.count(); ++i) {
                if (!valueArray.at(i).isString()) {
                    qWarning() << tr("Unknown value.");
                    return false;
                }
                valueStringList << valueArray.at(i).toString();
            }
            itemValue = valueStringList;
        }
        else {
            qWarning() << tr("Unknown value.");
            return false;
        }

        Qt::CheckState caseSensitivityValue = filterObject.value("caseSensitivity").toBool()?Qt::Checked:Qt::Unchecked;
        Qt::CheckState inverted = filterObject.value("inverted").toBool()?Qt::Checked:Qt::Unchecked;


        QModelIndex typeIndex = m_filtersModel->index(row, FiltersModel::TypeColumn, parentIndex);
        m_filtersModel->setData(typeIndex, FilterItem::Condition, Qt::EditRole);

        QModelIndex propertyIndex = m_filtersModel->index(row, FiltersModel::PropertyColumn, parentIndex);
        m_filtersModel->setData(propertyIndex, itemProperty, Qt::EditRole);

        QModelIndex operationIndex = m_filtersModel->index(row, FiltersModel::OperationColumn, parentIndex);
        m_filtersModel->setData(operationIndex, itemOperation, Qt::EditRole);

        QModelIndex valueIndex = m_filtersModel->index(row, FiltersModel::ValueColumn, parentIndex);
        m_filtersModel->setData(valueIndex, itemValue, Qt::EditRole);

        QModelIndex caseSensitivityIndex = m_filtersModel->index(row, FiltersModel::CaseSensitivityColumn, parentIndex);
        m_filtersModel->setData(caseSensitivityIndex, caseSensitivityValue, Qt::CheckStateRole);

        QModelIndex invertedIndex = m_filtersModel->index(row, FiltersModel::InvertedColumn, parentIndex);
        m_filtersModel->setData(invertedIndex, inverted, Qt::CheckStateRole);
    } else if (itemType.compare("and", Qt::CaseInsensitive) == 0) {
        QModelIndex typeIndex = m_filtersModel->index(row, FiltersModel::TypeColumn, parentIndex);
        m_filtersModel->setData(typeIndex, FilterItem::AndGroup, Qt::EditRole);

        QJsonArray childsArray = filterObject.value("childs").toArray();
        foreach (const QJsonValue &childItemValue, childsArray) {
            if (!childItemValue.isObject()) {
                qWarning() << tr("Child is not object.");
                return false;
            }
            createItemFromJson(childItemValue.toObject(), typeIndex);
        }

    } else if (itemType.compare("or", Qt::CaseInsensitive) == 0) {
        QModelIndex typeIndex = m_filtersModel->index(row, FiltersModel::TypeColumn, parentIndex);
        m_filtersModel->setData(typeIndex, FilterItem::OrGroup, Qt::EditRole);

        QJsonArray childsArray = filterObject.value("childs").toArray();
        foreach (const QJsonValue &childItemValue, childsArray) {
            if (!childItemValue.isObject()) {
                qWarning() << tr("Child is not object.");
                return false;
            }
            createItemFromJson(childItemValue.toObject(), typeIndex);
        }
    }

    return true;
}

void FiltersWidget::loadSettings()
{
    QSettings settigs;
    settigs.beginGroup("filtersConfig");
    QVariantList columnWidths = settigs.value("filterTableColumnWidths").toList();
    if (columnWidths.count() == m_filtersModel->columnCount()) {
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

