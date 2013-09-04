#ifndef RAWMESSAGESFORM_H
#define RAWMESSAGESFORM_H

#include <QWidget>

namespace Ui {
class RawMessagesForm;
}

class RawMessagesForm : public QWidget
{
    Q_OBJECT
    
public:
    explicit RawMessagesForm(QWidget *parent = 0);
    ~RawMessagesForm();
    Ui::RawMessagesForm *ui;
private:

public slots:
    void addMessage(QString message);



};

#endif // RAWMESSAGESFORM_H
