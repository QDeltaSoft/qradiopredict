#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    _db = new DatabaseApi;
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
        if(p->_use_antenna_pattern == 1)
        {
            this->ui->antennaCheckBox->setChecked(true);
        }
        if(p->_use_clutter == 1)
        {
            this->ui->clutterCheckBox->setChecked(true);
        }
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
    if(this->ui->antennaCheckBox->isChecked())
    {
        p->_use_antenna_pattern =1;
    }
    if(this->ui->clutterCheckBox->isChecked())
    {
        p->_use_clutter =1;
    }
    _db->savePrefs(p);
    delete p;
}
