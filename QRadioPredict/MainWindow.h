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


#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include "fgtelnet.h"
#include "aprs.h"
#include "aprsstation.h"
#include "aprsicon.h"
#include "aprspixmapitem.h"
#include "databaseapi.h"
#include "mobilestation.h"
#include "groundstation.h"
#include "flightplanpoints.h"
#include "flightgearprefs.h"
#include "plotpolygon.h"
#include "plotvalue.h"
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
#include "toolbox.h"
#include "ui_toolbox.h"
#include "connectionsuccessdialog.h"
#include "ui_connectionsuccessdialog.h"
#include "savedialog.h"
#include "loaddialog.h"

#include "Position.h"
#include "guts/Conversions.h"
#include "MapGraphicsView.h"
#include "MapGraphicsScene.h"
#include "tileSources/GridTileSource.h"
#include "tileSources/OSMTileSource.h"
#include "tileSources/CompositeTileSource.h"
#include "guts/CompositeTileSourceConfigurationWidget.h"
#include "CircleObject.h"
#include "PolygonObject.h"
#include "WeatherManager.h"

#include "radio/radiosystem.hxx"

#include <vector>
#include <math.h>

#include <QMainWindow>
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
#include <QSharedPointer>
#include <QtDebug>
#include <QImage>
#include <QGraphicsRectItem>
#include <QObject>


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
    void changePlotOpacity(int opacity);
    void savePlot(QString filename);
    void loadPlot(QString filename);
    void clearPlot();
    void openSavePlotDialog();
    void openLoadPlotDialog();



public slots:
    void mapClick(QPointF pos);
    void getMouseCoord(QPointF coord);
    void setMapItems(quint8 zoom);
    void saveMobile(MobileStation * m);
    void saveGroundStation(GroundStation * g);
    void saveFlightplan(FlightPlanPoints * fp);
    void deleteGroundStation(int id);
    void deleteFlightplan(int id);
    void sendFlightgearData();
    void enableStartButton();
    void startSignalUpdate();
    void stopSignalUpdate();
    void startStandalone();
    void stopStandalone();
    void moveMobile(double lon, double lat);
    void showSignalReading(double lon, double lat, int id_station, QString station_name, double freq, Signal*s);
    void newAPRSquery(quint8 zoom);
    void activateAPRS(bool active);
    void setReceived(QString data);
    void processAPRSData(AprsStation *st);
    void processRawAPRSData(QString data);
    void sequenceWaypoint();
    void plotCoverage(GroundStation * g);
    void drawPlot(double lon, double lat,
                  double lon1, double lat1,
                  double lon2, double lat2,
                  double lon3, double lat3,
                  double distance, double signal);
    void plottingFinished();
    void setPlotProgressBar(int ticks);
    void showPlotDistance();
    void setPlotDistance();


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
    QMap<QGraphicsPolygonItem*, PlotPolygon*> _plot_points;
    int _plot_opacity;
    QPixmap *_plot_pixmap;
    QGraphicsPixmapItem *_painted_pix;
    double _plot_progress_bar;
    double _plot_progress_bar_value;
    QVector<PlotValue*> *_plotvalues;

    Updater * _updater;
    FGRadio *_radio_subsystem;

    QMenu *_trayIconMenu;
    QAction *_restoreAction;
    QAction *_quitAction;
    QSystemTrayIcon *_trayIcon;


};

#endif // MAINWINDOW_H
