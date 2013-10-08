#include "connectionsuccessdialog.h"
#include "ui_connectionsuccessdialog.h"

ConnectionSuccessDialog::ConnectionSuccessDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectionSuccessDialog)
{
    ui->setupUi(this);
}

ConnectionSuccessDialog::~ConnectionSuccessDialog()
{
    delete ui;
}
