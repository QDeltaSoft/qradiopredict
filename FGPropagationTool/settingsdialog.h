#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
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
    
private:
    Ui::SettingsDialog *ui;
    DatabaseApi *_db;

    void fillEmptyFields();
};

#endif // SETTINGSDIALOG_H
