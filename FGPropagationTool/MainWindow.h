#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "fgtelnet.h"
#include "databaseapi.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    FGTelnet *_telnet;
    DatabaseApi *_db;

    
private slots:
    void on_actionExit_triggered();
    void connectionSuccess();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
