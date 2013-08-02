#include "groundstationform.h"
#include "ui_groundstationform.h"

GroundStationForm::GroundStationForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GroundStationForm)
{
    ui->setupUi(this);
}

GroundStationForm::~GroundStationForm()
{
    delete ui;
}
