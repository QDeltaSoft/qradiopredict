#include "loaddialog.h"
#include "ui_loaddialog.h"

LoadDialog::LoadDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoadDialog)
{
    ui->setupUi(this);
    connect(this, SIGNAL(accepted()),this, SLOT(getFilename()));
    connect(ui->browseButton,SIGNAL(clicked()),this,SLOT(openFileDialog()));
}

LoadDialog::~LoadDialog()
{
    delete ui;
}

void LoadDialog::getFilename()
{
    QString filename = ui->filenameEdit->text();
    emit filenameLoad(filename);
}

void LoadDialog::openFileDialog()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Open File"),"",tr("Files (*.*)"));
    ui->filenameEdit->setText(filename);
}
