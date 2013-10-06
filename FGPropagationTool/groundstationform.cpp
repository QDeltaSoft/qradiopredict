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

#include "groundstationform.h"
#include "ui_groundstationform.h"

GroundStationForm::GroundStationForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GroundStationForm)
{
    ui->setupUi(this);
    QObject::connect(ui->saveButton,SIGNAL(clicked()),this,SLOT(saveGround()));
    QObject::connect(ui->deleteButton,SIGNAL(clicked()),this,SLOT(deleteGround()));
    QObject::connect(ui->plotButton,SIGNAL(clicked()),this,SLOT(plot()));
}

GroundStationForm::~GroundStationForm()
{
    delete ui;
}


void GroundStationForm::saveGround()
{
    GroundStation *g = new GroundStation;
    g->id = ui->idEdit->text().toInt();
    g->id_session = 0;
    g->name = ui->nameEdit->text();
    g->heading_deg = ui->headingDegLineEdit->text().toDouble();
    g->elevation_feet = ui->elevationFeetLineEdit->text().toDouble();

    g->frequency = ui->frequencyEdit->text().toDouble();
    g->pitch_deg = ui->pitchDegLineEdit->text().toDouble();
    g->beacon_delay = ui->beaconDelayLineEdit->text().toInt();
    g->transmission_type = ui->transmissionTypeLineEdit->text().toInt();
    g->polarization = ui->polarizationLineEdit->text().toInt();
    g->rx_antenna_height = ui->rxAntennaHeightLineEdit->text().toDouble();
    g->rx_antenna_gain = ui->rxAntennaGainLineEdit->text().toDouble();
    g->rx_antenna_type = ui->rxAntennaTypeLineEdit->text();
    g->rx_line_losses = ui->rxLineLossesLineEdit->text().toDouble();
    g->rx_sensitivity = ui->rxSensitivityLineEdit->text().toDouble();
    g->tx_power_watt = ui->txPowerWattLineEdit->text().toDouble();
    g->tx_antenna_height = ui->txAntennaHeightLineEdit->text().toDouble();
    g->tx_antenna_gain = ui->txAntennaGainLineEdit->text().toDouble();
    g->tx_antenna_type = ui->txAntennaTypeLineEdit->text();
    g->tx_line_losses = ui->txLineLossesLineEdit->text().toDouble();
    g->enabled = 1;
    g->created_on = 0;
    emit haveData(g);
}


void GroundStationForm::deleteGround()
{

    unsigned id = ui->idEdit->text().toInt();
    emit delStation(id);
}

void GroundStationForm::plot()
{
    GroundStation *g = new GroundStation;
    g->id = ui->idEdit->text().toInt();
    g->id_session = 0;
    g->name = ui->nameEdit->text();
    g->heading_deg = ui->headingDegLineEdit->text().toDouble();
    g->elevation_feet = ui->elevationFeetLineEdit->text().toDouble();
    g->longitude = ui->longitudeEdit->text().toDouble();
    g->latitude = ui->latitudeEdit->text().toDouble();

    g->frequency = ui->frequencyEdit->text().toDouble();
    g->pitch_deg = ui->pitchDegLineEdit->text().toDouble();
    g->beacon_delay = ui->beaconDelayLineEdit->text().toInt();
    g->transmission_type = ui->transmissionTypeLineEdit->text().toInt();
    g->polarization = ui->polarizationLineEdit->text().toInt();
    g->rx_antenna_height = ui->rxAntennaHeightLineEdit->text().toDouble();
    g->rx_antenna_gain = ui->rxAntennaGainLineEdit->text().toDouble();
    g->rx_antenna_type = ui->rxAntennaTypeLineEdit->text();
    g->rx_line_losses = ui->rxLineLossesLineEdit->text().toDouble();
    g->rx_sensitivity = ui->rxSensitivityLineEdit->text().toDouble();
    g->tx_power_watt = ui->txPowerWattLineEdit->text().toDouble();
    g->tx_antenna_height = ui->txAntennaHeightLineEdit->text().toDouble();
    g->tx_antenna_gain = ui->txAntennaGainLineEdit->text().toDouble();
    g->tx_antenna_type = ui->txAntennaTypeLineEdit->text();
    g->tx_line_losses = ui->txLineLossesLineEdit->text().toDouble();
    g->enabled = 1;
    g->created_on = 0;
    emit plotStation(g);
}
