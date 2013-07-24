#include "toolbox.h"
#include "ui_toolbox.h"

toolbox::toolbox(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::toolbox)
{
    ui->setupUi(this);
}

toolbox::~toolbox()
{
    delete ui;
}
