// Written by Adrian Musceac YO8RZZ, started August 2013.
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
    int id = ui->idLineEdit->text().toInt();
    emit delFP(id);
}
