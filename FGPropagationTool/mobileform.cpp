#include "mobileform.h"
#include "ui_mobileform.h"

MobileForm::MobileForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MobileForm)
{
    ui->setupUi(this);
}

MobileForm::~MobileForm()
{
    delete ui;
}
