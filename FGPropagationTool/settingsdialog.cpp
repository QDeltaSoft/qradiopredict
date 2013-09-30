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
        if(p->_itm_radio_performance == 1)
        {
            this->ui->ITMCheckBox->setChecked(true);
        }
        this->ui->windowXEdit->setText(QString::number(p->_windowX));
        this->ui->windowYEdit->setText(QString::number(p->_windowY));
        this->ui->aprsServerEdit->setText(p->_aprs_server);
        this->ui->aprsRangeEdit->setText(QString::number(p->_aprs_filter_range));
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
    if(this->ui->clutterCheckBox->isChecked())
    {
        p->_use_clutter =1;
    }
    if(this->ui->ITMCheckBox->isChecked())
    {
        p->_itm_radio_performance =1;
    }
    p->_windowX = this->ui->windowXEdit->text().toInt();
    p->_windowY = this->ui->windowYEdit->text().toInt();
    p->_aprs_server = this->ui->aprsServerEdit->text();
    p->_aprs_filter_range = this->ui->aprsRangeEdit->text().toInt();
    _db->savePrefs(p);
    delete p;
}
