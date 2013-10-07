#ifndef GROUNDSTATIONFORM_H
#define GROUNDSTATIONFORM_H

#include <QWidget>
#include <QObject>
#include "groundstation.h"

namespace Ui {
class GroundStationForm;
}

class GroundStationForm : public QWidget
{
    Q_OBJECT
    
public:
    explicit GroundStationForm(QWidget *parent = 0);
    ~GroundStationForm();
    

    Ui::GroundStationForm *ui;
signals:
    void haveData(GroundStation * g);
    void delStation(int id);
    void plotStation(GroundStation *g);
private slots:
    void saveGround();
    void deleteGround();
    void plot();
};

#endif // GROUNDSTATIONFORM_H
