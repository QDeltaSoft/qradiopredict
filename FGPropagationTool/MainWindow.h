#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "fgtelnet.h"
#include "aprs.h"
#include "aprsstation.h"
#include "aprsicon.h"
#include "aprspixmapitem.h"
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
#include "rawmessagesform.h"
#include "ui_rawmessagesform.h"
#include <vector>
#include <math.h>
#include "MapGraphicsView.h"

#include "radio/radiosystem.hxx"

#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QMap>
#include <QThread>
#include <QTabBar>
#include <QGraphicsScene>
#include <QLineF>
#include <QPen>
#include <QBrush>
#include <QGraphicsTextItem>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QMessageBox>

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
    Aprs *_aprs;


signals:
    void newMessage(QString data);
    
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
    void connectToAPRS();
    void showRawAPRSMessages();
    void changeAPRSTimeFilter(int hours);



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
    void enableStartButton();
    void startSignalUpdate();
    void stopSignalUpdate();
    void startStandalone();
    void stopStandalone();
    void moveMobile(double lon, double lat);
    void showSignalReading(double lon, double lat, uint id_station,QString station_name,double freq,Signal*s);
    void newAPRSquery(quint8 zoom);
    void setReceived(QString data);
    void processAPRSData(AprsStation *st);
    void processRawAPRSData(QString data);
    void sequenceWaypoint();
    void plotCoverage(GroundStation * g);
    void drawPlot(double lon, double lat, double lon1, double lat1, double signal);


private:
    void restoreMapState();
    void createActions();
    void createTrayIcon();

    //void closeEvent(QCloseEvent *event);

    Ui::MainWindow *ui;
    MapGraphicsView *_view;
    int _placed_item_type;
    toolbox *_tb;
    QMap<QGraphicsPixmapItem *, QPointF> _map_mobiles;
    QMap<QGraphicsPixmapItem *, QPointF> _map_ground;
    QMap<QGraphicsPixmapItem *, QPointF> _map_fppos;
    QMap<AprsPixmapItem *, AprsIcon> _map_aprs;
    QMap<QGraphicsTextItem *, QPointF> _map_aprs_text;
    QVector<QDockWidget *> _docks;
    QVector<QString *> _raw_aprs_messages;
    QString _start_time;
    bool _show_signals;
    int _last_station_id;
    QVector<int> _station_ids;
    QVector<QGraphicsLineItem*> _signal_lines;
    QPointF _last_plot_point;
    Updater * _updater;
    FGRadio *_radio_subsystem;

    QMenu *_trayIconMenu;
    QAction *_restoreAction;
    QAction *_quitAction;
    QSystemTrayIcon *_trayIcon;


};

#endif // MAINWINDOW_H
