#ifndef RESULTITEMDELEGATE_H
#define RESULTITEMDELEGATE_H

#include <QStyledItemDelegate>

class ResultItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    ResultItemDelegate(QObject *parent = Q_NULLPTR);

    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
};

#endif // RESULTITEMDELEGATE_H
