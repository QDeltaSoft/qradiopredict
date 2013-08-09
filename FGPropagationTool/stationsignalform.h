#ifndef STATIONSIGNALFORM_H
#define STATIONSIGNALFORM_H

#include <QWidget>

namespace Ui {
class StationSignalForm;
}

class StationSignalForm : public QWidget
{
    Q_OBJECT
    
public:
    explicit StationSignalForm(QWidget *parent = 0);
    ~StationSignalForm();
    

    Ui::StationSignalForm *ui;
};

#endif // STATIONSIGNALFORM_H
