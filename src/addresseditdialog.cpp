#include "addresseditdialog.h"
#include "ui_addresseditdialog.h"
#include <QSettings>

AddressEditDialog::AddressEditDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddressEditDialog),
    m_completer(new QCompleter(this)),
    m_completerModel(new QStringListModel(this))
{
    ui->setupUi(this);
    ui->addressLineEdit->setInputMask(">AAAAAAAA;_");

    ui->actionAdd->setEnabled(false);
    ui->actionRemove->setEnabled(false);

    connect(ui->addressLineEdit, &QLineEdit::textChanged,
            this, &AddressEditDialog::checkAddress);

    connect(ui->addressesListWidget, &QListWidget::currentItemChanged,
            this, &AddressEditDialog::enableDisableListButtons);
    connect(ui->addressesListWidget->model(), &QAbstractItemModel::rowsInserted,
            this, &AddressEditDialog::enableDisableListButtons);
    connect(ui->addressesListWidget->model(), &QAbstractItemModel::rowsRemoved,
            this, &AddressEditDialog::enableDisableListButtons);

    connect(ui->removePushButton, &QPushButton::clicked,
            this, &AddressEditDialog::removeCurrentItem);
    connect(ui->actionRemove, &QAction::triggered,
            this, &AddressEditDialog::removeCurrentItem);

    connect(ui->addPushButton, &QPushButton::clicked,
            this, &AddressEditDialog::addAddress);
    connect(ui->actionAdd, &QAction::triggered,
            this, &AddressEditDialog::addAddress);
    connect(ui->addressLineEdit, &QLineEdit::returnPressed,
            this, &AddressEditDialog::addAddress);

    connect(this, &AddressEditDialog::accepted,
            this, &AddressEditDialog::saveLast100Addresses);

    connect(ui->upPushButton, &QPushButton::clicked,
            this, &AddressEditDialog::moveAddressUp);
    connect(ui->actionMoveUp, &QAction::triggered,
            this, &AddressEditDialog::moveAddressUp);

    connect(ui->downPushButton, &QPushButton::clicked,
            this, &AddressEditDialog::moveAddressDown);
    connect(ui->actionMoveDown, &QAction::triggered,
            this, &AddressEditDialog::moveAddressDown);

    m_completer->setModel(m_completerModel);
    m_completer->setFilterMode(Qt::MatchContains);
    ui->addressLineEdit->setCompleter(m_completer);

    ui->addressesListWidget->addAction(ui->actionRemove);
    ui->addressesListWidget->addAction(ui->actionMoveUp);
    ui->addressesListWidget->addAction(ui->actionMoveDown);
    ui->addressesListWidget->setContextMenuPolicy(Qt::ActionsContextMenu);

    updateCompletterModel();
}

AddressEditDialog::~AddressEditDialog()
{
    delete ui;
}

void AddressEditDialog::checkAddress()
{
    if (ui->addressLineEdit->text().length() == 8) {
        ui->addPushButton->setEnabled(true);
        ui->actionAdd->setEnabled(true);
    } else {
        ui->addPushButton->setEnabled(false);
        ui->actionAdd->setEnabled(false);
    }
}

void AddressEditDialog::removeCurrentItem()
{
    QListWidgetItem *currentItem = ui->addressesListWidget->currentItem();
    delete currentItem;
}

void AddressEditDialog::enableDisableListButtons()
{
    if (ui->addressesListWidget->count() > 0
            && ui->addressesListWidget->currentItem()) {
        ui->removePushButton->setEnabled(true);
        ui->actionRemove->setEnabled(true);

        QListWidgetItem *current = ui->addressesListWidget->currentItem();
        int currentRow = ui->addressesListWidget->row(current);
        if (currentRow > 0) {
            ui->upPushButton->setEnabled(true);
            ui->actionMoveUp->setEnabled(true);
        } else {
            ui->upPushButton->setEnabled(false);
            ui->actionMoveUp->setEnabled(false);
        }

        if (currentRow < ui->addressesListWidget->count() - 1) {
            ui->downPushButton->setEnabled(true);
            ui->actionMoveDown->setEnabled(true);
        } else {
            ui->downPushButton->setEnabled(false);
            ui->actionMoveDown->setEnabled(false);
        }

    } else {
        ui->removePushButton->setEnabled(false);
        ui->actionRemove->setEnabled(false);
    }
}

void AddressEditDialog::addAddress()
{
    if (ui->addressLineEdit->text().length() == 8) {
        ui->addressesListWidget->insertItem(0, ui->addressLineEdit->text());
        ui->addressLineEdit->clear();
        ui->addressLineEdit->setFocus();
    }
}

void AddressEditDialog::updateCompletterModel()
{
    QSettings settings;
    settings.beginGroup("addressesCompletter");
    m_completerModel->setStringList(settings.value("last100addresses", QStringList()).toStringList());
    settings.endGroup();
}

void AddressEditDialog::saveLast100Addresses()
{
    QStringList last100 = m_completerModel->stringList();

    foreach(const QString &address, addresses()) {
        last100.removeAll(address);
        if (last100.count() > 99) {
            last100.removeFirst();
        }
        last100.append(address);
    }

    QSettings settings;
    settings.beginGroup("addressesCompletter");
    settings.setValue("last100addresses", last100);
    settings.endGroup();

    m_completerModel->setStringList(last100);
}

void AddressEditDialog::moveAddressUp()
{
    QListWidgetItem *current = ui->addressesListWidget->currentItem();
    int currIndex = ui->addressesListWidget->row(current);

    QListWidgetItem *next = ui->addressesListWidget->item(ui->addressesListWidget->row(current) - 1);
    int nextIndex = ui->addressesListWidget->row(next);

    QListWidgetItem *temp = ui->addressesListWidget->takeItem(nextIndex);
    ui->addressesListWidget->insertItem(currIndex, temp);
    ui->addressesListWidget->insertItem(nextIndex, current);
}

void AddressEditDialog::moveAddressDown()
{
    QListWidgetItem *current = ui->addressesListWidget->currentItem();
    int currIndex = ui->addressesListWidget->row(current);

    QListWidgetItem *prev = ui->addressesListWidget->item(ui->addressesListWidget->row(current) + 1);
    int prevIndex = ui->addressesListWidget->row(prev);

    QListWidgetItem *temp = ui->addressesListWidget->takeItem(prevIndex);
    ui->addressesListWidget->insertItem(prevIndex, current);
    ui->addressesListWidget->insertItem(currIndex, temp);
}

QStringList AddressEditDialog::addresses() const
{
    QStringList result;
    for (int i = 0; i < ui->addressesListWidget->count(); ++i) {
        QListWidgetItem *item = ui->addressesListWidget->item(i);
        if (!result.contains(item->text()))
            result.append(item->text());
    }
    return result;
}

void AddressEditDialog::setAddresses(const QStringList &addresses)
{
    ui->addressesListWidget->clear();
    ui->addressesListWidget->addItems(addresses);

    ui->addressLineEdit->clear();

    enableDisableListButtons();
}

