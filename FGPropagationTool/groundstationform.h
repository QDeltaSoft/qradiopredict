#ifndef GROUNDSTATIONFORM_H
#define GROUNDSTATIONFORM_H

#include <QWidget>

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
};

#endif // GROUNDSTATIONFORM_H
