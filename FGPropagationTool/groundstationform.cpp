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
