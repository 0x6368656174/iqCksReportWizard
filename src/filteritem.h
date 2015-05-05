#ifndef FILTERITEM_H
#define FILTERITEM_H

#include <QObject>
#include <QVariant>

class FilterItem
{
    Q_GADGET
    Q_ENUMS(Operation)
public:
    enum Type
    {
        NotSetType,
        Condition,
        AndGroup,
        OrGroup
    };

    enum Properties
    {
        NotSetPorperty,
        DateTime,
        Direction,
        Channel,
        Number,
        AdditionalInfo,
        KS,
        Address,
        Sender,
        Text
    };

    enum Operation
    {
        NotSetOperation,
        Equals,
        NotEquals,
        StartsWith,
        EndsWith,
        Contains,
        Greater,
        GreaterOrEquals,
        Less,
        LessOrEquals
    };


    explicit FilterItem(FilterItem *parent = Q_NULLPTR);
    ~FilterItem();

    void addChildFilter(FilterItem *filter);
    void insertChildFilter(int row, FilterItem *filter);
    void removeChildFilter(FilterItem *filter);
    void removeChildFilterAt(int row);
    FilterItem *child(int row) const;

    int childCount() const;

    int row() const;

public:
    FilterItem *parentFilter() const;
    void setParentFilter(FilterItem *parentFilter);

    QList<FilterItem *> childFilters() const;
    void setChildFilters(const QList<FilterItem *> &childFilters);

    Type type() const;
    void setType(const Type &type);

    Properties property() const;
    void setProperty(const Properties &property);

    Operation operation() const;
    void setOperation(const Operation &operation);

    QVariant value() const;
    void setValue(const QVariant &value);

    Qt::CaseSensitivity caseSensitivity() const;
    void setCaseSensitivity(const Qt::CaseSensitivity &caseSensitivity);

private:
    FilterItem *m_parentFilter;
    QList<FilterItem *> m_childFilters;
    Type m_type;
    Properties m_property;
    Operation m_operation;
    QVariant m_value;
    Qt::CaseSensitivity m_caseSensitivity;
};

#endif // FILTERITEM_H
