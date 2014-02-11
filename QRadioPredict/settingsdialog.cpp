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
        if(p->_itm_radio_performance == 1)
        {
            this->ui->ITMCheckBox->setChecked(true);
        }
        this->ui->windowXEdit->setText(QString::number(p->_windowX));
        this->ui->windowYEdit->setText(QString::number(p->_windowY));
        this->ui->aprsServerEdit->setText(p->_aprs_server);
        this->ui->aprsRangeEdit->setText(QString::number(p->_aprs_filter_range));
        this->ui->plotRangeEdit->setText(QString::number(p->_plot_range));
        delete p;
    }
    prefs.clear();
}

void SettingsDialog::saveData()
{
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
    if(this->ui->ITMCheckBox->isChecked())
    {
        p->_itm_radio_performance =1;
    }
    else
    {
        p->_itm_radio_performance =0;
    }
    p->_windowX = this->ui->windowXEdit->text().toInt();
    p->_windowY = this->ui->windowYEdit->text().toInt();
    p->_aprs_server = this->ui->aprsServerEdit->text();
    p->_aprs_filter_range = this->ui->aprsRangeEdit->text().toInt();
    p->_plot_range = this->ui->plotRangeEdit->text().toInt();
    _db->savePrefs(p);
    emit updatePlotDistance();
    delete p;
}
