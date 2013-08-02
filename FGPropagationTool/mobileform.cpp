#include "mobileform.h"
#include "ui_mobileform.h"


MobileForm::MobileForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MobileForm)
{
    ui->setupUi(this);
    QObject::connect(ui->saveButton,SIGNAL(clicked()),this,SLOT(saveMobile()));
}

MobileForm::~MobileForm()
{
    delete ui;
}


void MobileForm::saveMobile()
{
    MobileStation *m = new MobileStation;
    m->id = ui->idEdit->text().toInt();
    m->id_session = 0;
    m->name = ui->nameEdit->text();
    m->heading_deg = ui->headingEdit->text().toDouble();
    m->elevation_feet = ui->elevationFeetEdit->text().toDouble();
    m->speed = ui->speedEdit->text().toDouble();
    m->frequency = ui->frequencyEdit->text().toDouble();
    m->terrain_following = ui->terrainFollowingEdit->text().toInt();
    emit haveData(m);
}
