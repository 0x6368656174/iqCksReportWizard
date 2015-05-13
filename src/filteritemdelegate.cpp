#include "filteritemdelegate.h"
#include <QComboBox>
#include <QDateTimeEdit>
#include <QSpinBox>
#include <QPointer>
#include <QStyleOptionComboBox>
#include <QApplication>
#include <QTimer>
#include <QDebug>
#include <QMessageBox>

FilterItemDelegate::FilterItemDelegate(QObject *parent) :
    QStyledItemDelegate(parent),
    m_mainWindow(Q_NULLPTR)
{
    m_groupsStrings[FilterItem::NotSetType] = tr("Not set");
    m_groupsStrings[FilterItem::Condition] = tr("Condition");
    m_groupsStrings[FilterItem::AndGroup] = tr("And");
    m_groupsStrings[FilterItem::OrGroup] = tr("Or");

    m_operationStrings[FilterItem::NotSetOperation] = tr("not set");
    m_operationStrings[FilterItem::Equals] = tr("equals");
    m_operationStrings[FilterItem::StartsWith] = tr("start with");
    m_operationStrings[FilterItem::EndsWith] = tr("end with");
    m_operationStrings[FilterItem::Contains] = tr("contains");
    m_operationStrings[FilterItem::Greater] = tr("greater");
    m_operationStrings[FilterItem::GreaterOrEquals] = tr("greater or equals");
    m_operationStrings[FilterItem::Less] = tr("less");
    m_operationStrings[FilterItem::LessOrEquals] = tr("less or equals");

    m_dateTimeOperationStrings[FilterItem::NotSetOperation] = m_operationStrings[FilterItem::NotSetOperation];
    m_dateTimeOperationStrings[FilterItem::Equals] = m_operationStrings[FilterItem::Equals];
    m_dateTimeOperationStrings[FilterItem::Greater] = m_operationStrings[FilterItem::Greater];
    m_dateTimeOperationStrings[FilterItem::GreaterOrEquals] = m_operationStrings[FilterItem::GreaterOrEquals];
    m_dateTimeOperationStrings[FilterItem::Less] = m_operationStrings[FilterItem::Less];
    m_dateTimeOperationStrings[FilterItem::LessOrEquals] = m_operationStrings[FilterItem::LessOrEquals];

    m_intOperationStrings[FilterItem::NotSetOperation] = m_operationStrings[FilterItem::NotSetOperation];
    m_intOperationStrings[FilterItem::Equals] = m_operationStrings[FilterItem::Equals];
    m_intOperationStrings[FilterItem::Greater] = m_operationStrings[FilterItem::Greater];
    m_intOperationStrings[FilterItem::GreaterOrEquals] = m_operationStrings[FilterItem::GreaterOrEquals];
    m_intOperationStrings[FilterItem::Less] = m_operationStrings[FilterItem::Less];
    m_intOperationStrings[FilterItem::LessOrEquals] = m_operationStrings[FilterItem::LessOrEquals];

    m_stringOperationStrings[FilterItem::NotSetOperation] = m_operationStrings[FilterItem::NotSetOperation];
    m_stringOperationStrings[FilterItem::Equals] = m_operationStrings[FilterItem::Equals];
    m_stringOperationStrings[FilterItem::StartsWith] = m_operationStrings[FilterItem::StartsWith];
    m_stringOperationStrings[FilterItem::EndsWith] = m_operationStrings[FilterItem::EndsWith];
    m_stringOperationStrings[FilterItem::Contains] = m_operationStrings[FilterItem::Contains];

    m_boolOperationStrings[FilterItem::NotSetOperation] = m_operationStrings[FilterItem::NotSetOperation];
    m_boolOperationStrings[FilterItem::Equals] = m_operationStrings[FilterItem::Equals];

    m_propertyStrings[FilterItem::NotSetPorperty] = tr("Not set");
    m_propertyStrings[FilterItem::ChannelName] = tr("Channel");
    m_propertyStrings[FilterItem::ChannelNumber] = tr("Number");
    m_propertyStrings[FilterItem::HeaderInfo] = tr("Additional info");
    m_propertyStrings[FilterItem::Priority] = tr("Priopity");
    //TODO временно не работают из-за того, что в таблице используются массивы
//    m_propertyStrings[FilterItem::Addresses] = tr("Address");
//    m_propertyStrings[FilterItem::Cc] = tr("Cc");
    m_propertyStrings[FilterItem::SenderTime] = tr("Sender time");
    m_propertyStrings[FilterItem::Sender] = tr("Sender");
    m_propertyStrings[FilterItem::SenderInfo] = tr("Sender Info");
    m_propertyStrings[FilterItem::Text] = tr("Text");
    m_propertyStrings[FilterItem::DateTime] = tr("Date time of archivate");
    m_propertyStrings[FilterItem::Direction] = tr("Direction");
    m_propertyStrings[FilterItem::ChannelId] = tr("Channel ID");
    m_propertyStrings[FilterItem::Journal] = tr("Journal");
    m_propertyStrings[FilterItem::SerialNumber] = tr("Serial number");
    m_propertyStrings[FilterItem::Svc] = tr("SVC");
    m_propertyStrings[FilterItem::RouteId] = tr("Route ID");
    m_propertyStrings[FilterItem::MessageType] = tr("Message Type");
}

FilterItemDelegate::~FilterItemDelegate()
{
}

QWidget *FilterItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    auto createComboBox = [parent](const QMap<int, QString> &values, bool invert){
        QComboBox *comboBox = new QComboBox(parent);
        QMapIterator<int, QString> valuesI (values);
        while (valuesI.hasNext()) {
            valuesI.next();
            if (invert
                    && valuesI.key() != FilterItem::NotSetOperation)
                comboBox->addItem(tr("NOT %0")
                                  .arg(valuesI.value()), valuesI.key());
            else
                comboBox->addItem(valuesI.value(), valuesI.key());
        }
        QTimer *timer = new QTimer (comboBox);
        QPointer<QComboBox> guarandComboBox (comboBox);
        timer->singleShot(10, comboBox, [guarandComboBox](){
            if (guarandComboBox)
                guarandComboBox->showPopup();
        });
        return comboBox;
    };

    switch (index.column()) {
    case FiltersModel::TypeColumn: {
        return createComboBox(m_groupsStrings, false);
        break;
    }
    case FiltersModel::OperationColumn : {
        FilterItem *item = static_cast<FilterItem *>(index.internalPointer());
        Q_CHECK_PTR(item);

        switch (item->propertyType()) {
        case QMetaType::QDateTime: {
            return createComboBox(m_dateTimeOperationStrings, item->inverted());
            break;
        }
        case QMetaType::Int: {
            return createComboBox(m_intOperationStrings, item->inverted());
            break;
        }
        case QMetaType::QString: {
            return createComboBox(m_stringOperationStrings, item->inverted());
            break;
        }
        case QMetaType::Bool: {
            return createComboBox(m_boolOperationStrings, item->inverted());
            break;
        }
        default:
            return createComboBox(m_operationStrings, item->inverted());
            break;
        }
        break;
    }
    case FiltersModel::PropertyColumn : {
        return createComboBox(m_propertyStrings, false);
        break;
    }
    case FiltersModel::ValueColumn: {
        FilterItem *item = static_cast<FilterItem *>(index.internalPointer());
        Q_CHECK_PTR(item);

        switch (item->propertyType()) {
        case QMetaType::QDateTime: {
            QDateTimeEdit *dateTimeEdit = new QDateTimeEdit(parent);
            dateTimeEdit->setDisplayFormat("dd.MM.yyyy hh:mm");
            dateTimeEdit->setCalendarPopup(true);
            return dateTimeEdit;
            break;
        }
        case QMetaType::Int: {
            QSpinBox *spinBox = new QSpinBox(parent);
            spinBox->setMaximum(9999999);
            return spinBox;
            break;
        }
        case QMetaType::Bool: {
            QMap<int, QString> boolValues;
            boolValues[false] = tr("FALSE");
            boolValues[true] = tr("TRUE");
            return createComboBox(boolValues, false);
            break;
        }
        default:
            return QStyledItemDelegate::createEditor(parent, option, index);
            break;
        }
        break;
    }
    default:
        return QStyledItemDelegate::createEditor(parent, option, index);
        break;
    }

    return Q_NULLPTR;
}

void FilterItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    switch (index.column()) {
    case FiltersModel::TypeColumn:
    case FiltersModel::OperationColumn:
    case FiltersModel::PropertyColumn: {
        int value = index.model()->data(index, Qt::EditRole).toInt();

        QComboBox *comboBox = static_cast<QComboBox*>(editor);
        int valueRow = comboBox->findData(value);
        comboBox->setCurrentIndex(valueRow);
        break;
    }
    case FiltersModel::ValueColumn: {
        QComboBox *comboBox = dynamic_cast<QComboBox *>(editor);
        if (comboBox) {
            FilterItem *item = static_cast<FilterItem *>(index.internalPointer());
            Q_CHECK_PTR(item);
            int valueRow = -1;
            switch (item->propertyType()) {
            case QMetaType::Bool: {
                if (item->value().toBool())
                    valueRow = 1;
                else
                    valueRow = 0;
                break;
            }
            default:
                int value = index.model()->data(index, Qt::EditRole).toInt();
                valueRow = comboBox->findData(value);
                break;
            }
            Q_ASSERT(valueRow != -1);
            comboBox->setCurrentIndex(valueRow);
            break;
        } else
            QStyledItemDelegate::setEditorData(editor, index);
    }
    }
}

void FilterItemDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    switch (index.column()) {
    case FiltersModel::TypeColumn:
    case FiltersModel::OperationColumn:
    case FiltersModel::PropertyColumn: {
        editor->setGeometry(option.rect);
        break;
    }
    case FiltersModel::ValueColumn: {
        FilterItem *item = static_cast<FilterItem *>(index.internalPointer());
        Q_CHECK_PTR(item);
        switch (item->propertyType()) {
        case QMetaType::QDateTime:
        case QMetaType::Int: {
            editor->setGeometry(option.rect);
            break;
        }
        default:
            QStyledItemDelegate::updateEditorGeometry(editor, option, index);
            break;
        }
    }
    default:
        QStyledItemDelegate::updateEditorGeometry(editor, option, index);
        break;
    }
}
QMainWindow *FilterItemDelegate::mainWindow() const
{
    return m_mainWindow;
}

void FilterItemDelegate::setMainWindow(QMainWindow *mainWindow)
{
    m_mainWindow = mainWindow;
}



void FilterItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    switch (index.column()) {
    case FiltersModel::TypeColumn:
    case FiltersModel::OperationColumn:
    case FiltersModel::PropertyColumn: {
        QComboBox *comboBox = static_cast<QComboBox*>(editor);
        int value = comboBox->currentData().toInt();

        if (index.column() == FiltersModel::TypeColumn) {
            if ((model->data(index).toInt() == FilterItem::OrGroup ||
                 model->data(index).toInt() == FilterItem::AndGroup) &&
                    (value == FilterItem::NotSetType ||
                     value == FilterItem::Condition)) {
                FilterItem *filter = static_cast<FilterItem *>(index.internalPointer());
                Q_CHECK_PTR(filter);
                if (filter->childCount() > 1) {
                    if (QMessageBox::question(mainWindow(),
                                              tr("Filter contains childs."),
                                              tr("Filter contains childs. This operation remove all child of filter. Continue?")) != QMessageBox::Yes)
                        return;
                }
            }
        }

        model->setData(index, value, Qt::EditRole);
        break;
    }
    default:
        QStyledItemDelegate::setModelData(editor, model, index);
        break;
    }
}


void FilterItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    auto paintComboBox = [painter, option, index, this](const QMap<int, QString> &values, bool invert){
        int value = index.model()->data(index, Qt::DisplayRole).toInt();
        QString text;
        if(values.contains(value))
            text = values[value];
        else
            text = values[0];

        if (invert
                && value != FilterItem::NotSetOperation)
            text = tr("NOT %0")
                    .arg(text);

        QStyleOptionViewItem textOption (option);
        QRect textRect = option.rect;
        textRect.setWidth(option.rect.width() - 20);
        textOption.rect = textRect;
        textOption.text = text;
        QStyledItemDelegate::paint(painter, textOption, QModelIndex());

#ifndef Q_OS_WIN
        QStyleOptionViewItem backgroundOption (option);
        QRect backgroundRect = option.rect;
        backgroundRect.setX(option.rect.x() + option.rect.width() - 20);
        backgroundOption.rect = backgroundRect;
        QApplication::style()->drawPrimitive(QStyle::PE_PanelItemViewItem,
                                             &backgroundOption, painter);
#endif

        QStyleOptionComboBox comboBoxOption;
        comboBoxOption.palette = option.palette;
        QRect comboBoxRect = option.rect;
#ifdef Q_OS_WIN
        comboBoxRect.setX(option.rect.x() + option.rect.width() - 20);
#endif
        comboBoxOption.rect = comboBoxRect;
        comboBoxOption.frame = false;
        comboBoxOption.state = option.state;
        comboBoxOption.currentText = text;
        QApplication::style()->drawComplexControl(QStyle::CC_ComboBox,
                                                  &comboBoxOption, painter);
    };

    FilterItem::Type groupType = static_cast<FilterItem::Type>(index.model()->data(index.model()->index(index.row(),
                                                                                                        FiltersModel::TypeColumn,
                                                                                                        index.parent()),
                                                                                   Qt::EditRole).toInt());
    switch (index.column()) {
    case FiltersModel::TypeColumn:
        paintComboBox(m_groupsStrings, false);
        break;
    case FiltersModel::OperationColumn:
        if (groupType == FilterItem::Condition) {
            FilterItem *item = static_cast<FilterItem *>(index.internalPointer());
            paintComboBox(m_operationStrings, item->inverted());
        } else {
            QStyledItemDelegate::paint(painter, option, index);
        }
        break;
    case FiltersModel::PropertyColumn:
        if (groupType == FilterItem::Condition)
            paintComboBox(m_propertyStrings, false);
        else
            QStyledItemDelegate::paint(painter, option, index);
        break;
    case FiltersModel::ValueColumn: {
        FilterItem *item = static_cast<FilterItem *>(index.internalPointer());
        Q_CHECK_PTR(item);
        switch (item->propertyType()) {
        case QMetaType::QDateTime: {
            QStyleOptionViewItem valueOption (option);
            valueOption.text = index.model()->data(index).toDateTime().toString("dd.MM.yyyy hh:mm");
            QStyledItemDelegate::paint(painter, valueOption, QModelIndex());
            break;
        }
        case QMetaType::Bool: {
            QStyleOptionViewItem valueOption (option);
            valueOption.text = index.model()->data(index).toBool()?tr("TRUE"):tr("FALSE");
            QStyledItemDelegate::paint(painter, valueOption, QModelIndex());
            break;
        }
        default:
            QStyledItemDelegate::paint(painter, option, index);
            break;
        }
        break;
    }
    case FiltersModel::InvertedColumn: {
        FilterItem *item = static_cast<FilterItem *>(index.internalPointer());
        Q_CHECK_PTR(item);
        if (groupType == FilterItem::Condition)
            QStyledItemDelegate::paint(painter, option, index);
        else {
            QStyleOptionViewItem notCheckOption (option);
            notCheckOption.features = QStyleOptionViewItem::None;
            QStyledItemDelegate::paint(painter, notCheckOption, QModelIndex());
        }
        break;
    }
    case FiltersModel::CaseSensitivityColumn: {
        FilterItem *item = static_cast<FilterItem *>(index.internalPointer());
        Q_CHECK_PTR(item);
        if (groupType == FilterItem::Condition
                && item->propertyType() == QMetaType::QString)
            QStyledItemDelegate::paint(painter, option, index);
        else {
            QStyleOptionViewItem notCheckOption (option);
            notCheckOption.features = QStyleOptionViewItem::None;
            QStyledItemDelegate::paint(painter, notCheckOption, QModelIndex());
        }
        break;
    }
    default:
        QStyledItemDelegate::paint(painter, option, index);
        break;
    }
}
