#ifndef SAVEDIALOG_H
#define SAVEDIALOG_H

#include <QDialog>
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
