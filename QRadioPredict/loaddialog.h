#ifndef LOADDIALOG_H
#define LOADDIALOG_H

#include <QDialog>
#include <QString>
#include <QFileDialog>
#include <QObject>

namespace Ui {
class LoadDialog;
}

class LoadDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit LoadDialog(QWidget *parent = 0);
    ~LoadDialog();

signals:
    void filenameLoad(QString filename);

private slots:
    void getFilename();
    void openFileDialog();
    
private:
    Ui::LoadDialog *ui;
};

#endif // LOADDIALOG_H
