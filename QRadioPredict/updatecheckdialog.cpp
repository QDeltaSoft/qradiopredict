#include "updatecheckdialog.h"
#include "ui_updatecheckdialog.h"

UpdateCheckDialog::UpdateCheckDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UpdateCheckDialog)
{
    ui->setupUi(this);
}

UpdateCheckDialog::~UpdateCheckDialog()
{
    delete ui;
}


void UpdateCheckDialog::noUpdateAvailable()
{
    this->ui->labelText->setText("No update available");
    this->show();
}

void UpdateCheckDialog::updateCheckerError()
{
    this->ui->labelText->setText("Error while checking for updates");
    this->show();
}

void UpdateCheckDialog::updateAvailable(QString version)
{
    this->ui->labelText->setText(version+" available");
    this->show();
}
