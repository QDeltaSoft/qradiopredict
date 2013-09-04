#include "rawmessagesform.h"
#include "ui_rawmessagesform.h"

RawMessagesForm::RawMessagesForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RawMessagesForm)
{
    ui->setupUi(this);
}

RawMessagesForm::~RawMessagesForm()
{
    delete ui;
}


void RawMessagesForm::addMessage(QString message)
{
    ui->messagesTextEdit->append(message);
}
