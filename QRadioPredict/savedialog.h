// Written by Adrian Musceac YO8RZZ, started August 2013.
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

#ifndef SAVEDIALOG_H
#define SAVEDIALOG_H

#include <QDialog>
#include <QObject>
#include <QString>
#include <QFileDialog>

namespace Ui {
class SaveDialog;
}

class SaveDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit SaveDialog(QWidget *parent = 0);
    ~SaveDialog();
    Ui::SaveDialog *ui;

signals:
    void filenameSave(QString filename);

private slots:
    void getFilename();
    void openFileDialog();
    
private:

};

#endif // SAVEDIALOG_H
