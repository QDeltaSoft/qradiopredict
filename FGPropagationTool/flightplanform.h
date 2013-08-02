#ifndef FLIGHTPLANFORM_H
#define FLIGHTPLANFORM_H

#include <QWidget>
#include "flightplanpoints.h"

namespace Ui {
class FlightplanForm;
}

class FlightplanForm : public QWidget
{
    Q_OBJECT
    
public:
    explicit FlightplanForm(QWidget *parent = 0);
    ~FlightplanForm();
    

    Ui::FlightplanForm *ui;

signals:
    void haveData(FlightPlanPoints * fp);
private slots:
    void saveFP();
};

#endif // FLIGHTPLANFORM_H
