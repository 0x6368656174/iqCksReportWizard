#ifndef ADDRESSEDITDIALOG_H
#define ADDRESSEDITDIALOG_H

#include <QDialog>
#include <QCompleter>
#include <QStringListModel>

namespace Ui {
class AddressEditDialog;
}

class AddressEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddressEditDialog(QWidget *parent = 0);
    ~AddressEditDialog();

    QStringList addresses() const;
    void setAddresses(const QStringList &addresses);

private:
    void checkAddress();
    void removeCurrentItem();
    void enableDisableListButtons();
    void addAddress();
    void updateCompletterModel();
    void saveLast100Addresses();
    void moveAddressUp();
    void moveAddressDown();

private:
    Ui::AddressEditDialog *ui;
    QCompleter *m_completer;
    QStringListModel *m_completerModel;
};

#endif // ADDRESSEDITDIALOG_H
