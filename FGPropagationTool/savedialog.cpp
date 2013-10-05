#include "savedialog.h"
#include "ui_savedialog.h"

SaveDialog::SaveDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SaveDialog)
{
    ui->setupUi(this);
    connect(this, SIGNAL(accepted()),this, SLOT(getFilename()));
    connect(ui->browseButton,SIGNAL(clicked()),this,SLOT(openFileDialog()));
}

SaveDialog::~SaveDialog()
{
    delete ui;
}

void SaveDialog::getFilename()
{
    QString filename = ui->filenameEdit->text();
    emit filenameSave(filename);
}

void SaveDialog::openFileDialog()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("Open File"),"",tr("Files (*.*)"));
    ui->filenameEdit->setText(filename);
}
