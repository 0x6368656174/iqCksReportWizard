#include "resultwidget.h"
#include "ui_resultwidget.h"
#include <QSettings>
#include "resultitemdelegate.h"

ResultWidget::ResultWidget(QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::ResultWidget),
    m_findModel(new IqOrmModel<ArchiveRecord>(this)),
    m_verticalSplitter(new QSplitter(this)),
    m_horizontalSplitter(new QSplitter(this))
{
    ui->setupUi(this);
    m_findModel->showProperty("objectId", tr("ID"));
    m_findModel->showProperty("dateTime", tr("Archive Date And Time"));
    m_findModel->showProperty("channelName", tr("Channel Name"));
    m_findModel->showProperty("channelNumber", tr("Channel Number"));
    m_findModel->showProperty("headerInfo", tr("Header Additional Info"));
    m_findModel->showProperty("addresses", tr("Addresses"));
    m_findModel->showProperty("cc", tr("Copies"));
    m_findModel->showProperty("sender", tr("Sender"));
    m_findModel->showProperty("text", tr("Text"));

    ui->resultTableView->setItemDelegate(new ResultItemDelegate(ui->resultTableView));
    ui->resultTableView->setModel(m_findModel);

    m_horizontalSplitter->setOrientation(Qt::Horizontal);
    m_horizontalSplitter->addWidget(ui->textGroupBox);
    m_horizontalSplitter->addWidget(ui->journalGroupBox);

    m_verticalSplitter->setOrientation(Qt::Vertical);
    m_verticalSplitter->addWidget(ui->resultTableView);
    m_verticalSplitter->addWidget(m_horizontalSplitter);

    ui->mainLayout->addWidget(m_verticalSplitter);

    connect(ui->resultTableView->selectionModel(), &QItemSelectionModel::currentChanged,
            this, &ResultWidget::showItemText);
}

ResultWidget::~ResultWidget()
{
    delete ui;
}
IqOrmModel<ArchiveRecord> *ResultWidget::findModel() const
{
    return m_findModel;
}

void ResultWidget::loadSettings()
{
    QSettings settigs;
    settigs.beginGroup("resultConfig");
    QVariantList columnWidths = settigs.value("resultTableColumnWidths").toList();
    if (columnWidths.count() >= m_findModel->columnCount()) {
        for (int i = 0; i < m_findModel->columnCount(); ++i) {
            ui->resultTableView->setColumnWidth(i, columnWidths[i].toInt());
        }
    }

    QList<int> verticalSplitterSizes;
    foreach (const QVariant &size, settigs.value("verticalSplitterSizes").toList()) {
        verticalSplitterSizes << size.toInt();
    }
    m_verticalSplitter->setSizes(verticalSplitterSizes);

    QList<int> horizontalSplitterSizes;
    foreach (const QVariant &size, settigs.value("horizontalSplitterSizes").toList()) {
        horizontalSplitterSizes << size.toInt();
    }
    m_horizontalSplitter->setSizes(horizontalSplitterSizes);

    settigs.endGroup();
}

void ResultWidget::saveSettings()
{
    QSettings settigs;
    settigs.beginGroup("resultConfig");
    QVariantList columnWidths;
    for (int i = 0; i < m_findModel->columnCount(); ++i) {
        columnWidths << ui->resultTableView->columnWidth(i);
    }
    settigs.setValue("resultTableColumnWidths", columnWidths);

    QVariantList verticalSplitterSizes;
    foreach (int size, m_verticalSplitter->sizes()) {
        verticalSplitterSizes << size;
    }
    settigs.setValue("verticalSplitterSizes", verticalSplitterSizes);

    QVariantList horizontalSplitterSizes;
    foreach (int size, m_horizontalSplitter->sizes()) {
        horizontalSplitterSizes << size;
    }
    settigs.setValue("horizontalSplitterSizes", horizontalSplitterSizes);

    settigs.endGroup();
}

void ResultWidget::showItemText(const QModelIndex &currentIndex, const QModelIndex &previosIndex)
{
    Q_UNUSED(previosIndex);
    ArchiveRecord *record = m_findModel->get(currentIndex.row());
    if(record) {
        ui->textTextEdit->setText(record->text());
        ui->journalTextEdit->setText(record->journal());
    } else {
        ui->textTextEdit->clear();
        ui->journalTextEdit->clear();
    }
}
