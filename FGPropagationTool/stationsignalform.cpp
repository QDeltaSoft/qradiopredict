#include "stationsignalform.h"
#include "ui_stationsignalform.h"

StationSignalForm::StationSignalForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StationSignalForm)
{
    ui->setupUi(this);
}

StationSignalForm::~StationSignalForm()
{
    delete ui;
}
