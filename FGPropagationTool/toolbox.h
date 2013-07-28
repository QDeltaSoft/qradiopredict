#ifndef TOOLBOX_H
#define TOOLBOX_H

#include <QWidget>

namespace Ui {
class toolbox;
}

class toolbox : public QWidget
{
    Q_OBJECT
    
public:
    explicit toolbox(QWidget *parent = 0);
    ~toolbox();
    

    Ui::toolbox *ui;
};

#endif // TOOLBOX_H
