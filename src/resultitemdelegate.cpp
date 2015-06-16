#include "resultitemdelegate.h"
#include <QStringList>

ResultItemDelegate::ResultItemDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}

void ResultItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QVariant data = index.data();

    switch (data.type()) {
    case QVariant::StringList: {
        QStyleOptionViewItem valueOption (option);
        valueOption.text = data.toStringList().join(", ");
        QStyledItemDelegate::paint(painter, valueOption, QModelIndex());
        break;
    }
    default:
        QStyledItemDelegate::paint(painter, option, index);
        break;
    }
}

