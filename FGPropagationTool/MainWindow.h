#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "fgtelnet.h"
#include "databaseapi.h"
#include "mobilestation.h"
#include "groundstation.h"
#include "flightplanpoints.h"
#include "util.h"
#include "fgremote.h"
#include "updater.h"
#include "signal.h"
#include "toolbox.h"
#include "mobileform.h"
#include "ui_mobileform.h"
#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "groundstationform.h"
#include "ui_groundstationform.h"
#include "stationsignalform.h"
#include "ui_stationsignalform.h"
#include "flightplanform.h"
#include "ui_flightplanform.h"
#include <vector>
#include <math.h>
#include "MapGraphicsView.h"

#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QMap>
#include <QThread>
#include <QTabBar>
#include <QGraphicsScene>

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
    FGRemote *_remote;

    
private slots:
    void on_actionExit_triggered();
    void connectionSuccess();
    void connectionFailure();
    void startFGFS();
    void showSettingsDialog();
    void setMobileType();
    void setGroundType();
    void setFPType();
    void showEditBoxes();
    void clearLeftDocks();

public slots:
    void mapClick(QPointF pos);
    void getMouseCoord(QPointF coord);
    void setMapItems(quint8 zoom);
    void saveMobile(MobileStation * m);
    void saveGroundStation(GroundStation * g);
    void saveFlightplan(FlightPlanPoints * fp);
    void deleteGroundStation(unsigned id);
    void deleteFlightplan(unsigned id);
    void sendFlightgearData();
    void startSignalUpdate();
    void moveMobile(double lon, double lat);
    void showSignalReading(double lon, double lat, uint id_station,QString station_name,double freq,Signal*s);

private:
    void restoreMapState();

    Ui::MainWindow *ui;
    MapGraphicsView *_view;
    int _placed_item_type;
    toolbox *_tb;
    QMap<QGraphicsPixmapItem *, QPointF> _map_mobiles;
    QMap<QGraphicsPixmapItem *, QPointF> _map_ground;
    QMap<QGraphicsPixmapItem *, QPointF> _map_fppos;
    QVector<QDockWidget *> _docks;
    QString _start_time;
    bool _show_signals;
    int _last_station_id;
    QVector<int> _station_ids;
};

#endif // MAINWINDOW_H
