#include "filtersmodel.h"
#include <QDateTime>

FiltersModel::FiltersModel(QObject *parent) :
    QAbstractItemModel(parent),
    m_mainRootFilter(new FilterItem()),
    m_rootFilter(new FilterItem(m_mainRootFilter))
{
    m_mainRootFilter->setType(FilterItem::AndGroup);
    m_mainRootFilter->addChildFilter(m_rootFilter);
}

FiltersModel::~FiltersModel()
{
    delete m_mainRootFilter;
    m_mainRootFilter = Q_NULLPTR;
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
            case FilterItem::NotSetType:
                return QVariant();
                break;
            case FilterItem::Condition:
                return filter->property();
                break;
            case FilterItem::OrGroup:
                return QVariant();
                break;
            case FilterItem::AndGroup:
                return QVariant();
                break;
            }
            break;
        }

        case OperationColumn: {
            switch (filter->type()) {
            case FilterItem::NotSetType:
                return QVariant();
                break;
            case FilterItem::Condition:
                return filter->operation();
                break;
            case FilterItem::OrGroup:
                return QVariant();
                break;
            case FilterItem::AndGroup:
                return QVariant();
                break;
            }
            break;
        }
        case ValueColumn: {
            switch (filter->type()) {
            case FilterItem::NotSetType:
                return QVariant();
                break;
            case FilterItem::Condition:
                return filter->value();
                break;
            case FilterItem::OrGroup:
                return QVariant();
                break;
            case FilterItem::AndGroup:
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
    return 6;
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
