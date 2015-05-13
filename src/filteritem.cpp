#include "filteritem.h"

FilterItem::FilterItem(FilterItem *parent):
    m_parentFilter(parent),
    m_type(NotSetType),
    m_property(NotSetPorperty),
    m_operation(NotSetOperation),
    m_value(QVariant()),
    m_caseSensitivity(Qt::CaseInsensitive),
    m_inverted(false)
{
}

FilterItem::~FilterItem()
{
    qDeleteAll(m_childFilters);
    m_childFilters.clear();
}

FilterItem *FilterItem::parentFilter() const
{
    return m_parentFilter;
}

void FilterItem::setParentFilter(FilterItem *parentFilter)
{
    m_parentFilter = parentFilter;
}

QList<FilterItem *> FilterItem::childFilters() const
{
    return m_childFilters;
}

void FilterItem::setChildFilters(const QList<FilterItem *> &childFilters)
{
    qDeleteAll(m_childFilters);

    Q_ASSERT(m_type != NotSetType);
    Q_ASSERT(m_type != Condition);

    m_childFilters = childFilters;

    foreach (FilterItem *filter, m_childFilters) {
        Q_CHECK_PTR(filter);
        filter->setParentFilter(this);
    }
}

void FilterItem::addChildFilter(FilterItem *filter)
{
    Q_CHECK_PTR(filter);
    Q_ASSERT(m_type != NotSetType);
    Q_ASSERT(m_type != Condition);
    m_childFilters.append(filter);
    filter->setParentFilter(this);
}

void FilterItem::insertChildFilter(int row, FilterItem *filter)
{
    Q_CHECK_PTR(filter);
    Q_ASSERT(m_type != NotSetType);
    Q_ASSERT(m_type != Condition);
    m_childFilters.insert(row, filter);
    filter->setParentFilter(this);
}

void FilterItem::removeChildFilter(FilterItem *filter)
{
    Q_CHECK_PTR(filter);
    Q_ASSERT(m_type != NotSetType);
    Q_ASSERT(m_type != Condition);
    m_childFilters.removeOne(filter);
}

void FilterItem::removeChildFilterAt(int row)
{
    Q_ASSERT(row > -1);
    Q_ASSERT(row < m_childFilters.count());
    Q_ASSERT(m_type != NotSetType);
    Q_ASSERT(m_type != Condition);
    m_childFilters.removeAt(row);
}

FilterItem *FilterItem::child(int row) const
{
    Q_ASSERT(row > -1);
    Q_ASSERT(row < m_childFilters.count());
    return m_childFilters.at(row);
}

int FilterItem::childCount() const
{
    return m_childFilters.count();
}

QMetaType::Type FilterItem::propertyType() const
{
    return propertyType(property());
}

QMetaType::Type FilterItem::propertyType(Properties property) const
{
    switch (property) {
    case NotSetPorperty:
        return QMetaType::UnknownType;
        break;
    case DateTime:
        return QMetaType::QDateTime;
        break;
    case ChannelNumber:
    case Priority:
    case Direction:
    case ChannelId:
    case SerialNumber:
    case RouteId:
    case MessageType:
        return QMetaType::Int;
        break;
    case Svc:
        return QMetaType::Bool;
        break;
    default:
        return QMetaType::QString;
        break;
    }

    return QMetaType::UnknownType;
}

int FilterItem::row() const
{
    if (!parentFilter())
        return 0;

    return parentFilter()->childFilters().indexOf(const_cast<FilterItem *>(this));
}

FilterItem::Type FilterItem::type() const
{
    return m_type;
}

void FilterItem::setType(const Type &groupType)
{
    if (m_type != groupType) {
        m_type = groupType;
    }
}

FilterItem::Properties FilterItem::property() const
{
    return m_property;
}

void FilterItem::setProperty(const Properties &property)
{
    m_property = property;
}

FilterItem::Operation FilterItem::operation() const
{
    return m_operation;
}

void FilterItem::setOperation(const Operation &operation)
{
    m_operation = operation;
}

QVariant FilterItem::value() const
{
    return m_value;
}

void FilterItem::setValue(const QVariant &value)
{
    m_value = value;
}

Qt::CaseSensitivity FilterItem::caseSensitivity() const
{
    return m_caseSensitivity;
}

void FilterItem::setCaseSensitivity(const Qt::CaseSensitivity &caseSensitivity)
{
    m_caseSensitivity = caseSensitivity;
}

bool FilterItem::inverted() const
{
    return m_inverted;
}

void FilterItem::setInverted(bool invert)
{
    m_inverted = invert;
}


