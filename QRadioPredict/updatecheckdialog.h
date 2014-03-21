#ifndef UPDATECHECKDIALOG_H
#define UPDATECHECKDIALOG_H

#include <QDialog>

namespace Ui {
class UpdateCheckDialog;
}

class UpdateCheckDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit UpdateCheckDialog(QWidget *parent = 0);
    ~UpdateCheckDialog();

public slots:
    void noUpdateAvailable();
    void updateAvailable(QString version);
    void updateCheckerError();
    
private:
    Ui::UpdateCheckDialog *ui;
};

#endif // UPDATECHECKDIALOG_H
