#include "filtersmodel.h"
#include <QDateTime>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <IqOrmAndGroupFilter>
#include <IqOrmOrGroupFilter>
#include <IqOrmFilter>
#include <QMessageBox>

FiltersModel::FiltersModel(QObject *parent) :
    QAbstractItemModel(parent),
    m_mainRootFilter(new FilterItem()),
    m_rootFilter(new FilterItem(m_mainRootFilter))
{

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

    m_operationsNames[FilterItem::Equals] = "equals";
    m_operationsNames[FilterItem::StartsWith] = "startsWith";
    m_operationsNames[FilterItem::EndsWith] = "endsWith";
    m_operationsNames[FilterItem::Contains] = "contains";
    m_operationsNames[FilterItem::Greater] = "greaterThan";
    m_operationsNames[FilterItem::GreaterOrEquals] = "greaterOrEquals";
    m_operationsNames[FilterItem::Less] = "lessThan";
    m_operationsNames[FilterItem::LessOrEquals] = "lessOrEquals";
    m_operationsNames[FilterItem::Overlap] = "overlap";
    m_operationsNames[FilterItem::NotSetOperation] = "";

    m_reportTemplateNames[FilterItem::StartDateTime] = "startDateTime";
    m_reportTemplateNames[FilterItem::EndDateTime] = "endDateTime";
    m_reportTemplateNames[FilterItem::NotSetTemplate] = "";

    m_mainRootFilter->setType(FilterItem::AndGroup);
    m_mainRootFilter->addChildFilter(m_rootFilter);
}

FiltersModel::~FiltersModel()
{
    delete m_mainRootFilter;
    m_mainRootFilter = Q_NULLPTR;
}

bool FiltersModel::loadFromFile(const QString &fileName)
{
    setData(rootIndex(FiltersModel::TypeColumn), FilterItem::NotSetType, Qt::EditRole);

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

    createItemFromJson(rootObject, invisibleRootItemIndex(0));

    return true;
}

bool FiltersModel::saveToFile(const QString &fileName) const
{
    QFile file (fileName);
    if (!file.open(QFile::WriteOnly | QFile::Truncate)) {
        qWarning() << tr("Error on open file \"%0\" for write.")
                      .arg(fileName);
        return false;
    }

    QJsonObject rootObject = createFilterJson(rootFilter());
    QJsonDocument jsonDoc;
    jsonDoc.setObject(rootObject);


    file.write(jsonDoc.toJson());
    return true;
}

IqOrmAbstractFilter *FiltersModel::createFilter(bool *abort, QString *error) const
{
    return createFilter(rootFilter(), abort, error);
}

IqOrmAbstractFilter *FiltersModel::createFilter(const FilterItem *filterItem, bool *abort, QString *error) const
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

        if (filterItem->property() == FilterItem::Cc
                || filterItem->property() == FilterItem::Addresses) {
            QStringList errorAddresses;
            bool findErrorAddresses = false;
            foreach (const QString &string, filterItem->value().toStringList()) {
                if(string.length() < 8) {
                    findErrorAddresses = true;
                    errorAddresses << "<span style=\"color: red\">" + string + "</span>";
                } else
                    errorAddresses << string;
            }
            if (findErrorAddresses) {
                if (error)
                    *error = tr("Error on addresses in \"%0\" condition: %1.\n"
                                "Address must be 8 litter.")
                        .arg(filterItem->property() == FilterItem::Cc?tr("Cc"):tr("Addresses"))
                        .arg(errorAddresses.join(", "));

                *abort = true;
                return Q_NULLPTR;
            }
        }

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
            bool childAbbort = false;
            IqOrmAbstractFilter *childResultFilter = createFilter(childFilter, &childAbbort, error);
            if (childAbbort) {
                resultFilter->deleteLater();
                *abort = true;
                return Q_NULLPTR;
            }
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
            bool childAbbort = false;
            IqOrmAbstractFilter *childResultFilter = createFilter(childFilter, &childAbbort, error);
            if (childAbbort) {
                resultFilter->deleteLater();
                *abort = true;
                return Q_NULLPTR;
            }
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

QVariant FiltersModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();


    if (role != Qt::DisplayRole
            && role != Qt::EditRole
            && role != Qt::CheckStateRole)
        return QVariant();

    FilterItem *filter = static_cast<FilterItem *>(index.internalPointer());

    switch (role) {
    case Qt::CheckStateRole: {
        if (index.column() == InvertedColumn) {
            if (filter->inverted())
                return Qt::Checked;
            else
                return Qt::Unchecked;
        } else if (index.column() == CaseSensitivityColumn) {
            if (filter->caseSensitivity() == Qt::CaseSensitive)
                return Qt::Checked;
            else
                return Qt::Unchecked;
        }
        else
            return QVariant();
        break;
    }
    case Qt::DisplayRole:
    case Qt::EditRole: {
        switch (index.column()) {
        case TypeColumn: {
            return filter->type();
            break;
        }

        case PropertyColumn: {
            switch (filter->type()) {
            case FilterItem::Condition:
                return filter->property();
                break;
            default:
                return QVariant();
                break;
            }
            break;
        }

        case OperationColumn: {
            switch (filter->type()) {
            case FilterItem::Condition:
                return filter->operation();
                break;
            default:
                return QVariant();
                break;
            }
            break;
        }
        case ValueColumn: {
            switch (filter->type()) {
            case FilterItem::Condition:
                return filter->value();
                break;
            default:
                return QVariant();
                break;
            }
            break;
        }
        case ReportTemplateColumn: {
            switch (filter->type()) {
            case FilterItem::Condition:
                return filter->reportTemplate();
                break;
            default:
                return QVariant();
                break;
            }
            break;
        }
        case InvertedColumn: {
            return QVariant();
            break;
        }
        case CaseSensitivityColumn: {
            return QVariant();
            break;
        }
        }
        break;
    }
    }

    return QVariant();
}

Qt::ItemFlags FiltersModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    FilterItem *filter = static_cast<FilterItem *>(index.internalPointer());

    switch (index.column()) {
    case TypeColumn:
        return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
        break;
    case InvertedColumn:
        if (filter->type() == FilterItem::Condition) {
            return QAbstractItemModel::flags(index) | Qt::ItemIsUserCheckable;
        } else {
            return QAbstractItemModel::flags(index);
        }
        break;
    case CaseSensitivityColumn:
        if (filter->type() == FilterItem::Condition) {
            return QAbstractItemModel::flags(index) | Qt::ItemIsUserCheckable;
        } else {
            return QAbstractItemModel::flags(index);
        }
        break;
    default:
        if (filter->type() == FilterItem::Condition) {
            return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
        } else {
            return QAbstractItemModel::flags(index);
        }
        break;
    }

    return QAbstractItemModel::flags(index);
}

QVariant FiltersModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (static_cast<Columns>(section)) {
        case TypeColumn:
            return tr("Type");
            break;
        case PropertyColumn:
            return tr("Property");
            break;
        case OperationColumn:
            return tr("Operation");
            break;
        case ValueColumn:
            return tr("Value");
            break;
        case ReportTemplateColumn:
            return tr("Report Template");
            break;
        case InvertedColumn:
            return tr("Inverted");
            break;
        case CaseSensitivityColumn:
            return tr("Case Sensitive");
            break;
        }
    }

    return QVariant();
}

QModelIndex FiltersModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    FilterItem *parentFilter;

    if (!parent.isValid())
        parentFilter = m_mainRootFilter;
    else
        parentFilter = static_cast<FilterItem *>(parent.internalPointer());

    FilterItem *childFilter = parentFilter->child(row);
    if (childFilter)
        return createIndex(row, column, childFilter);

    return QModelIndex();
}

QModelIndex FiltersModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    FilterItem *childFilter = static_cast<FilterItem *>(index.internalPointer());
    FilterItem *parentFilter = childFilter->parentFilter();

    if (parentFilter == m_mainRootFilter)
        return QModelIndex();

    return createIndex(parentFilter->row(), 0, parentFilter);
}

int FiltersModel::rowCount(const QModelIndex &parent) const
{
    FilterItem *parentFilter;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentFilter = m_mainRootFilter;
    else
        parentFilter = static_cast<FilterItem *>(parent.internalPointer());

    return parentFilter->childCount();
}

int FiltersModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 7;
}

FilterItem *FiltersModel::rootFilter() const
{
    return m_rootFilter;
}

QModelIndex FiltersModel::rootIndex(int column) const
{
    return createIndex(0, column, m_rootFilter);
}

QModelIndex FiltersModel::invisibleRootItemIndex(int column) const
{
    return createIndex(0, column, m_mainRootFilter);
}

QModelIndex FiltersModel::indexForFilter(FilterItem *filter)
{
    if (!filter)
        return QModelIndex();

    if (!filter->parentFilter())
        return QModelIndex();

    return createIndex(filter->parentFilter()->childFilters().indexOf(filter), 0, filter);
}

QJsonObject FiltersModel::createFilterJson(const FilterItem *filterItem) const
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
        default:
            Q_ASSERT(m_operationsNames.contains(filterItem->operation()));
            operation = m_operationsNames[filterItem->operation()];
        }
        resultObject.insert("operation", operation);

        if (filterItem->value().type() == QVariant::Bool)
            resultObject.insert("value", filterItem->value().toBool());
        else if (filterItem->value().type() == QVariant::StringList) {
            QJsonArray valueArray;
            foreach (const QString &string, filterItem->value().toStringList()) {
                valueArray.append(QJsonValue(string));
            }
            resultObject.insert("value", valueArray);
        } else if (filterItem->value().canConvert<double>()
                 && filterItem->value().type() != QVariant::String
                 && filterItem->value().type() != QVariant::ByteArray)
            resultObject.insert("value", filterItem->value().toDouble());
        else if (filterItem->value().canConvert<QString>())
            resultObject.insert("value", filterItem->value().toString());
        else
            qWarning() << tr("Error to save filter value to Json.");

        QString reportTemplate;
        switch (filterItem->reportTemplate()) {
        case FilterItem::NotSetTemplate:
            break;
        default:
            Q_ASSERT(m_reportTemplateNames.contains(filterItem->reportTemplate()));
            reportTemplate = m_reportTemplateNames[filterItem->reportTemplate()];
        }
        resultObject.insert("reportTemplate", reportTemplate);

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

bool FiltersModel::createItemFromJson(const QJsonObject &filterObject, const QModelIndex &parentIndex)
{
    int row = rowCount(parentIndex) - 1;
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

        QHashIterator<FilterItem::Operation, QString> operationsI(m_operationsNames);
        bool operationFound = false;
        while (operationsI.hasNext()) {
            operationsI.next();
            if (operation.compare(operationsI.value(), Qt::CaseInsensitive) == 0) {
                itemOperation = operationsI.key();
                operationFound = true;
                break;
            }
        }

        if (!operationFound)
            qWarning() << tr("Unknown operation \"%0\".")
                          .arg(property);

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

        QString reportTemplate = filterObject.value("reportTemplate").toString();
        FilterItem::ReportTemplate itemReportTemplate = FilterItem::NotSetTemplate;

        QHashIterator<FilterItem::ReportTemplate, QString> reportTemplateI(m_reportTemplateNames);
        bool reportTemplateFound = false;
        while (reportTemplateI.hasNext()) {
            reportTemplateI.next();
            if (reportTemplate.compare(reportTemplateI.value(), Qt::CaseInsensitive) == 0) {
                itemReportTemplate = reportTemplateI.key();
                reportTemplateFound = true;
                break;
            }
        }

        if (!reportTemplateFound)
            qWarning() << tr("Unknown report template \"%0\".")
                          .arg(property);

        Qt::CheckState caseSensitivityValue = filterObject.value("caseSensitivity").toBool()?Qt::Checked:Qt::Unchecked;
        Qt::CheckState inverted = filterObject.value("inverted").toBool()?Qt::Checked:Qt::Unchecked;

        QModelIndex typeIndex = index(row, FiltersModel::TypeColumn, parentIndex);
        setData(typeIndex, FilterItem::Condition, Qt::EditRole);

        QModelIndex propertyIndex = index(row, FiltersModel::PropertyColumn, parentIndex);
        setData(propertyIndex, itemProperty, Qt::EditRole);

        QModelIndex operationIndex = index(row, FiltersModel::OperationColumn, parentIndex);
        setData(operationIndex, itemOperation, Qt::EditRole);

        QModelIndex valueIndex = index(row, FiltersModel::ValueColumn, parentIndex);
        setData(valueIndex, itemValue, Qt::EditRole);

        QModelIndex reportTemplateIndex = index(row, FiltersModel::ReportTemplateColumn, parentIndex);
        setData(reportTemplateIndex, itemReportTemplate, Qt::EditRole);

        QModelIndex caseSensitivityIndex = index(row, FiltersModel::CaseSensitivityColumn, parentIndex);
        setData(caseSensitivityIndex, caseSensitivityValue, Qt::CheckStateRole);

        QModelIndex invertedIndex = index(row, FiltersModel::InvertedColumn, parentIndex);
        setData(invertedIndex, inverted, Qt::CheckStateRole);
    } else if (itemType.compare("and", Qt::CaseInsensitive) == 0) {
        QModelIndex typeIndex = index(row, FiltersModel::TypeColumn, parentIndex);
        setData(typeIndex, FilterItem::AndGroup, Qt::EditRole);

        QJsonArray childsArray = filterObject.value("childs").toArray();
        foreach (const QJsonValue &childItemValue, childsArray) {
            if (!childItemValue.isObject()) {
                qWarning() << tr("Child is not object.");
                return false;
            }
            createItemFromJson(childItemValue.toObject(), typeIndex);
        }

    } else if (itemType.compare("or", Qt::CaseInsensitive) == 0) {
        QModelIndex typeIndex = index(row, FiltersModel::TypeColumn, parentIndex);
        setData(typeIndex, FilterItem::OrGroup, Qt::EditRole);

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

bool FiltersModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    if (role != Qt::EditRole
            && role != Qt::CheckStateRole)
        return false;

    FilterItem *filter = static_cast<FilterItem *>(index.internalPointer());
    Q_CHECK_PTR(filter);

    switch (role) {
    case Qt::CheckStateRole: {
        if (index.column() == InvertedColumn)
            filter->setInverted(value == Qt::Checked);
        else if (index.column() == CaseSensitivityColumn)
            filter->setCaseSensitivity(value == Qt::Checked?Qt::CaseSensitive:Qt::CaseInsensitive);
        else
            return false;
        break;
    }
    case Qt::EditRole : {
        if (index.column() == TypeColumn) {
            switch (value.toInt()) {
            case FilterItem::NotSetType:
            case FilterItem::Condition:
                if (filter->childCount() > 0) {
                    beginRemoveRows(index, 0, filter->childCount() - 1);
                    filter->setChildFilters(QList<FilterItem *>());
                    endRemoveRows();
                }
                break;
            default:
                break;
            }
        }

        FilterItem::Type oldGroupType = filter->type();

        switch (index.column()) {
        case TypeColumn:
            filter->setType(static_cast<FilterItem::Type>(value.toInt()));
            break;
        case PropertyColumn: {
            QMetaType::Type oldType = filter->propertyType();
            QMetaType::Type newType = filter->propertyType(static_cast<FilterItem::Properties>(value.toInt()));
            if (newType == QMetaType::QDateTime
                    && oldType != newType)
                filter->setValue(QDateTime::currentDateTime());
            if (newType == QMetaType::Int
                    && oldType != newType)
                filter->setValue(0);
            if (newType == QMetaType::Bool
                    && oldType != newType)
                filter->setValue(false);
            if (newType == QMetaType::QString
                    && oldType != newType)
                filter->setValue("");

            switch (newType) {
            case QMetaType::QDateTime:
            case QMetaType::Int:
                switch (filter->operation()) {
                case FilterItem::NotSetOperation:
                case FilterItem::Equals:
                case FilterItem::Greater:
                case FilterItem::GreaterOrEquals:
                case FilterItem::Less:
                case FilterItem::LessOrEquals:
                    break;
                default:
                    filter->setOperation(FilterItem::NotSetOperation);
                    break;
                }
                break;
            case QMetaType::Bool:
                switch (filter->operation()) {
                case FilterItem::NotSetOperation:
                case FilterItem::Equals:
                    break;
                default:
                    filter->setOperation(FilterItem::NotSetOperation);
                    break;
                }
            case QMetaType::QString:
                switch (filter->operation()) {
                case FilterItem::NotSetOperation:
                case FilterItem::Equals:
                case FilterItem::StartsWith:
                case FilterItem::EndsWith:
                case FilterItem::Contains:
                    break;
                default:
                    filter->setOperation(FilterItem::NotSetOperation);
                    break;
                }
            default:
                break;
            }

            filter->setProperty(static_cast<FilterItem::Properties>(value.toInt()));
            break;
        }
        case OperationColumn:
            filter->setOperation(static_cast<FilterItem::Operation>(value.toInt()));
            break;
        case ValueColumn:
            filter->setValue(value);
            break;
        case ReportTemplateColumn:
            filter->setReportTemplate(static_cast<FilterItem::ReportTemplate>(value.toInt()));
            break;
        case CaseSensitivityColumn:
            break;
        }

        if (index.column() == TypeColumn) {
            switch (value.toInt()) {
            case FilterItem::AndGroup:
            case FilterItem::OrGroup:
                if (oldGroupType == FilterItem::NotSetType
                        || oldGroupType == FilterItem::Condition) {
                    beginInsertRows(index, 0, 0);
                    filter->addChildFilter(new FilterItem(filter));
                    endInsertRows();
                }
            default:
                break;
            }
        }

        //Приведем в порядок NotSet фильтры
        FilterItem *parentFilter = filter->parentFilter();
        if (parentFilter) {
            QModelIndex parentIndex = indexForFilter(parentFilter);
            if (parentIndex.isValid()) {
                for (int i = parentFilter->childCount() - 1; i > -1; i--) {
                    if (parentFilter->child(i)->type() == FilterItem::NotSetType) {
                        beginRemoveRows(parentIndex, i, i);
                        parentFilter->removeChildFilterAt(i);
                        endRemoveRows();
                    }
                }
                beginInsertRows(parentIndex, parentFilter->childCount(), parentFilter->childCount());
                parentFilter->addChildFilter(new FilterItem(parentFilter));
                endInsertRows();
            }
        }
        break;
    }
    }

    emit dataChanged(this->index(index.row(), 0, index.parent()), this->index(index.row(), columnCount() - 1, index.parent()));

    return true;
}


bool FiltersModel::insertRows(int row, int count, const QModelIndex &parent)
{
    if (row < 0)
        return false;
    if (row > rowCount())
        return false;
    if (count < 0)
        return false;

    beginInsertRows(parent, row, row + count -1);

    FilterItem *filter = static_cast<FilterItem *>(parent.internalPointer());
    Q_CHECK_PTR(filter);

    filter->insertChildFilter(row, new FilterItem(filter));
    endInsertRows();

    return true;
}

bool FiltersModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (row < 0)
        return false;
    if (row + count > rowCount() - 1)
        return false;
    if (count < 0)
        return false;

    beginInsertRows(parent, row, row + count -1);
    FilterItem *filter = static_cast<FilterItem *>(parent.internalPointer());
    Q_CHECK_PTR(filter);

    filter->removeChildFilterAt(row);
    endRemoveRows();

    return true;
}
