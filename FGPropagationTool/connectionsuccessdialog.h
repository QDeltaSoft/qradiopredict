#ifndef CONNECTIONSUCCESSDIALOG_H
#define CONNECTIONSUCCESSDIALOG_H

#include <QDialog>

namespace Ui {
class ConnectionSuccessDialog;
}

class ConnectionSuccessDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit ConnectionSuccessDialog(QWidget *parent = 0);
    ~ConnectionSuccessDialog();
    

    Ui::ConnectionSuccessDialog *ui;
};

#endif // CONNECTIONSUCCESSDIALOG_H
