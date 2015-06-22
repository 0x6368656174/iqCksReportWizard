#ifndef STRINGLISTDELEGATEWIDGET_H
#define STRINGLISTDELEGATEWIDGET_H

#include <QLineEdit>
#include <QToolButton>
#include <QMainWindow>
#include "addresseditdialog.h"

class StringListDelegateWidget : public QLineEdit
{
    Q_OBJECT

public:
    explicit StringListDelegateWidget(QWidget *parent = 0);
    ~StringListDelegateWidget();

    void setValue(const QStringList &value);
    QStringList value() const;

    QMainWindow *mainWindow() const;
    void setMainWindow(QMainWindow *mainWindow);

private:
    void validate(const QString &text);
    void updateValue(const QString &text);
    void showEditDialog();

private:
    QToolButton *m_toolButton;
    QStringList m_value;
    QString m_oldValidText;
    QMainWindow *m_mainWindow;
    AddressEditDialog *m_editDialog;
};

#endif // STRINGLISTDELEGATEWIDGET_H
