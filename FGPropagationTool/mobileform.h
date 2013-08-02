#ifndef MOBILEFORM_H
#define MOBILEFORM_H

#include <QWidget>

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
};

#endif // MOBILEFORM_H
