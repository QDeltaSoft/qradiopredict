#include "flightplanform.h"
#include "ui_flightplanform.h"

FlightplanForm::FlightplanForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FlightplanForm)
{
    ui->setupUi(this);
    QObject::connect(ui->saveButton,SIGNAL(clicked()),this,SLOT(saveFP()));
    QObject::connect(ui->deleteButton,SIGNAL(clicked()),this,SLOT(deleteFP()));
}

FlightplanForm::~FlightplanForm()
{
    delete ui;
}


void FlightplanForm::saveFP()
{
    FlightPlanPoints *fp = new FlightPlanPoints;
    fp->id = ui->idLineEdit->text().toInt();
    fp->id_session = 0;

    fp->altitude = ui->altitudeLineEdit->text().toDouble();

    emit haveData(fp);
}


void FlightplanForm::deleteFP()
{
    unsigned id = ui->idLineEdit->text().toInt();
    emit delFP(id);
}
