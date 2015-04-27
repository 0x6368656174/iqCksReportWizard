#include "configdialog.h"
#include "ui_configdialog.h"

#include <QSettings>

ConfigDialog::ConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigDialog)
{
    ui->setupUi(this);
    loadSettings();
    connect(this, &ConfigDialog::accepted,
            this, &ConfigDialog::saveSettings);
}

ConfigDialog::~ConfigDialog()
{
    delete ui;
}

void ConfigDialog::loadSettings()
{
    QSettings settings;
    settings.beginGroup("databaseConfig");
    ui->hostLineEdit->setText(settings.value("host", "localhost").toString());
    ui->portSpinBox->setValue(settings.value("port", 5432).toInt());
    ui->userLineEdit->setText(settings.value("user", "postgres").toString());
    ui->passwordLineEdit->setText(settings.value("password").toString());
    ui->databaseNameLineEdit->setText(settings.value("databaseName", "cks").toString());
    settings.endGroup();
}

void ConfigDialog::saveSettings()
{
    QSettings settings;
    settings.beginGroup("databaseConfig");
    settings.setValue("host", ui->hostLineEdit->text());
    settings.setValue("port", ui->portSpinBox->value());
    settings.setValue("user", ui->userLineEdit->text());
    settings.setValue("password", ui->passwordLineEdit->text());
    settings.setValue("databaseName", ui->databaseNameLineEdit->text());
}
