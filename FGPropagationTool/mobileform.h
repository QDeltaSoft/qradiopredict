#ifndef MOBILEFORM_H
#define MOBILEFORM_H

#include <QWidget>
#include "mobilestation.h"

namespace Ui {
class MobileForm;
}

class MobileForm : public QWidget
{
    Q_OBJECT
    
public:
    explicit MobileForm(QWidget *parent = 0);
    ~MobileForm();
    

    Ui::MobileForm *ui;
signals:
    void haveData(MobileStation * m);
private slots:
    void saveMobile();
};

#endif // MOBILEFORM_H
