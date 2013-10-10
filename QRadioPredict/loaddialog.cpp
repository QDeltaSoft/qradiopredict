// Written by Adrian Musceac YO8RZZ at gmail dot com, started August 2013.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

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
