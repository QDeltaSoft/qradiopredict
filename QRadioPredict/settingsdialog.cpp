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

#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(DatabaseApi *db, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    _db = db;
    QObject::connect(ui->buttonBox,SIGNAL(accepted()),this,SLOT(saveData()));
    this->fillEmptyFields();
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}


void SettingsDialog::fillEmptyFields()
{
    QVector<FlightgearPrefs *> prefs = _db->select_prefs();
    if(prefs.size()>0)
    {
        FlightgearPrefs *p = prefs[0];
        this->ui->fgfsEdit->setText(p->_fgfs_bin);
        this->ui->fgdataEdit->setText( p->_fgdata_path);
        this->ui->sceneryEdit->setText(p->_scenery_path);
        this->ui->aircraftEdit->setText(p->_aircraft);
        this->ui->airportEdit->setText(p->_airport);
        this->ui->srtmPathLineEdit->setText(p->_srtm_path);
        this->ui->shapePathLineEdit->setText(p->_shapefile_path);
        if(p->_use_antenna_pattern == 1)
        {
            this->ui->antennaCheckBox->setChecked(true);
        }
        if(p->_use_clutter == 1)
        {
            this->ui->clutterCheckBox->setChecked(true);
        }
        if(p->_use_ITWOM == 1)
        {
            this->ui->ITWOMCheckBox->setChecked(true);
        }
        if(p->_scale_with_distance == 1)
        {
            this->ui->scaleWithDistanceCheckBox->setChecked(true);
        }
        if(p->_itm_radio_performance == 1)
        {
            this->ui->ITMCheckBox->setChecked(true);
        }
        this->ui->windowXEdit->setValue(p->_windowX);
        this->ui->windowYEdit->setValue(p->_windowY);
        this->ui->aprsServerEdit->setText(p->_aprs_server);
        this->ui->aprsSettingsEdit->setText(p->_aprs_settings);
        this->ui->aprsRangeEdit->setValue(p->_aprs_filter_range);
        this->ui->plotRangeEdit->setValue(p->_plot_range);
        this->ui->initialLatEdit->setText(QString::number(p->_init_latitude));
        this->ui->initialLongEdit->setText(QString::number(p->_init_longitude));
        delete p;
    }
    prefs.clear();
}

void SettingsDialog::saveData()
{
    qDebug( "Saving Settings" );

    FlightgearPrefs *p = new FlightgearPrefs;
    p->_fgfs_bin = this->ui->fgfsEdit->text();
    p->_fgdata_path= this->ui->fgdataEdit->text();
    p->_scenery_path= this->ui->sceneryEdit->text();
    p->_aircraft = this->ui->aircraftEdit->text();
    p->_airport = this->ui->airportEdit->text();
    p->_srtm_path = this->ui->srtmPathLineEdit->text();
    p->_shapefile_path = this->ui->shapePathLineEdit->text();
    if(this->ui->antennaCheckBox->isChecked())
    {
        p->_use_antenna_pattern =1;
    }
    else
    {
        p->_use_antenna_pattern =0;
    }
    if(this->ui->clutterCheckBox->isChecked())
    {
        p->_use_clutter =1;
    }
    else
    {
        p->_use_clutter =0;
    }
    if(this->ui->ITWOMCheckBox->isChecked())
    {
        p->_use_ITWOM =1;
    }
    else
    {
        p->_use_ITWOM =0;
    }
    if(this->ui->scaleWithDistanceCheckBox->isChecked())
    {
        p->_scale_with_distance =1;
    }
    else
    {
        p->_scale_with_distance =0;
    }
    if(this->ui->ITMCheckBox->isChecked())
    {
        p->_itm_radio_performance =1;
    }
    else
    {
        p->_itm_radio_performance =0;
    }
    p->_windowX = this->ui->windowXEdit->value();
    p->_windowY = this->ui->windowYEdit->value();
    p->_aprs_server = this->ui->aprsServerEdit->text();
    p->_aprs_settings = this->ui->aprsSettingsEdit->text();
    p->_aprs_filter_range = this->ui->aprsRangeEdit->value();
    p->_plot_range = this->ui->plotRangeEdit->text().toInt();
    p->_init_latitude = this->ui->initialLatEdit->text().toDouble();
    p->_init_longitude = this->ui->initialLongEdit->text().toDouble();
    _db->savePrefs(p);
    emit updatePlotDistance();
    delete p;
}

void SettingsDialog::chooseDirectory(QString desc,QLineEdit *field){
    QString path = QFileDialog::getExistingDirectory(this, desc, ".", QFileDialog::ReadOnly);
    field->setText(path);
}

void SettingsDialog::chooseFile(QString desc,QLineEdit *field){
    QString path = QFileDialog::getOpenFileName(this, desc, ".");
    field->setText(path);
}

void SettingsDialog::on_fgDataDirDialog_clicked()
{
    chooseDirectory(tr("Flightgear Data Directory"), this->ui->fgdataEdit);
}

void SettingsDialog::on_sceneryEditDialog_clicked()
{
    chooseDirectory(tr("Scenery Directory"), this->ui->sceneryEdit);
}

void SettingsDialog::on_srtmPathLineEditDialog_clicked()
{
    chooseDirectory(tr("SRTM Directory"), this->ui->srtmPathLineEdit);
}

void SettingsDialog::on_shapePathLineEditDialog_clicked()
{
    chooseDirectory(tr("Shapefile Directory"), this->ui->shapePathLineEdit);
}

void SettingsDialog::on_fgfsEditDialog_clicked()
{
    chooseFile(tr("Flightgear Executable"), this->ui->fgfsEdit);
}

void SettingsDialog::on_AircraftEditDialog_clicked()
{
    chooseFile(tr("Aircraft "), this->ui->aircraftEdit);
}

void SettingsDialog::on_airportEditDialog_clicked()
{
    chooseFile(tr("Airport "), this->ui->airportEdit);
}
