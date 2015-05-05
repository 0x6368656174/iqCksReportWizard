#ifndef FILTERSMODEL_H
#define FILTERSMODEL_H

#include <QAbstractItemModel>
#include "filteritem.h"

class FiltersModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    enum Columns {
        TypeColumn,
        PropertyColumn,
        OperationColumn,
        ValueColumn,
        CaseSensitivityColumn
    };

    explicit FiltersModel(QObject *parent = 0);
    ~FiltersModel();

    virtual QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role) Q_DECL_OVERRIDE;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
    virtual QVariant headerData(int section, Qt::Orientation orientation,
                                int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    virtual QModelIndex index(int row, int column,
                              const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    virtual QModelIndex parent(const QModelIndex &index) const Q_DECL_OVERRIDE;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    virtual bool insertRows(int row, int count, const QModelIndex &parent) Q_DECL_OVERRIDE;
    virtual bool removeRows(int row, int count, const QModelIndex &parent) Q_DECL_OVERRIDE;

public:
    FilterItem *rootFilter() const;
    QModelIndex rootIndex(int column = 0) const;
    QModelIndex invisibleRootItemIndex(int column = 0) const;

private:
    QModelIndex indexForFilter(FilterItem *filter);

private:
    FilterItem *m_mainRootFilter;
    FilterItem *m_rootFilter;
};

#endif // FILTERSMODEL_H
