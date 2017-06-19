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

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QObject>
#include <QFileDialog>
#include "databaseapi.h"

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit SettingsDialog(DatabaseApi *db, QWidget *parent = 0);
    ~SettingsDialog();

private slots:
    void saveData();
    void chooseDirectory(QString desc, QLineEdit *field);
    void chooseFile(QString desc,QLineEdit *field);

    //buttons for PATH dialog
    void on_fgDataDirDialog_clicked();
    void on_sceneryEditDialog_clicked();
    void on_srtmPathLineEditDialog_clicked();
    void on_shapePathLineEditDialog_clicked();

    //buttons for FILE dialog
    void on_fgfsEditDialog_clicked();
    void on_AircraftEditDialog_clicked();
    void on_airportEditDialog_clicked();
    void on_buttonBox_accepted();

signals:
    void updatePlotDistance();
    
private:
    Ui::SettingsDialog *ui;
    DatabaseApi *_db;

    void fillEmptyFields();
};

#endif // SETTINGSDIALOG_H
