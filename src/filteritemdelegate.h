#ifndef FILTERITEMDELEGATE_H
#define FILTERITEMDELEGATE_H

#include <QStyledItemDelegate>
#include <QMap>
#include "filtersmodel.h"
#include <QMainWindow>

class FilterItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    FilterItemDelegate(QObject * parent = Q_NULLPTR);
    ~FilterItemDelegate();

    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
    virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
    virtual void setEditorData(QWidget *editor, const QModelIndex &index) const Q_DECL_OVERRIDE;
    virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const Q_DECL_OVERRIDE;
    virtual void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;

    QMainWindow *mainWindow() const;
    void setMainWindow(QMainWindow *mainWindow);

private:
    QMap<int, QString> m_groupsStrings;
    QMap<int, QString> m_operationStrings;
    QMap<int, QString> m_dateTimeOperationStrings;
    QMap<int, QString> m_intOperationStrings;
    QMap<int, QString> m_stringOperationStrings;
    QMap<int, QString> m_boolOperationStrings;
    QMap<int, QString> m_propertyStrings;
    QMainWindow *m_mainWindow;
};

#endif // FILTERITEMDELEGATE_H
