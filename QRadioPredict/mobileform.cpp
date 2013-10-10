// Written by Adrian Musceac YO8RZZ at gmail dot com, started August 2013.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

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
