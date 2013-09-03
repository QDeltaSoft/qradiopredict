#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "toolbox.h"
#include "ui_toolbox.h"
#include "connectionsuccessdialog.h"
#include "ui_connectionsuccessdialog.h"

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

#include <QSharedPointer>
#include <QtDebug>
#include <QThread>
#include <QImage>
#include <QGraphicsRectItem>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    _telnet = new FGTelnet;
    _db = new DatabaseApi;
    _remote = new FGRemote(_telnet, _db);
    _show_signals = false;
    _last_station_id = -1;

    ui->setupUi(this);
    //!!!!!!!! connections must always come after setupUi!!!
    QObject::connect(ui->actionConnect_to_Flightgear,SIGNAL(triggered()),this->_telnet,SLOT(connectToFGFS()));
    QObject::connect(ui->actionStart_Flightgear,SIGNAL(triggered()),this,SLOT(startFGFS()));
    QObject::connect(ui->action_Settings,SIGNAL(triggered()),this,SLOT(showSettingsDialog()));
    QObject::connect(ui->actionConnect_APRS,SIGNAL(triggered()),this,SLOT(connectToAPRS()));
    QObject::connect(this->_telnet,SIGNAL(connectedToFGFS()),this,SLOT(connectionSuccess()));
    QObject::connect(this->_telnet,SIGNAL(connectionFailure()),this,SLOT(connectionFailure()));

    //Setup the MapGraphics scene and view
    MapGraphicsScene * scene = new MapGraphicsScene(this);
    MapGraphicsView * view = new MapGraphicsView(scene,this);
    _view=view;
    //The view will be our central widget
    this->setCentralWidget(view);

    //Setup some tile sources
    QSharedPointer<OSMTileSource> osmTiles(new OSMTileSource(OSMTileSource::OSMTiles), &QObject::deleteLater);
    QSharedPointer<OSMTileSource> aerialTiles(new OSMTileSource(OSMTileSource::MapQuestAerialTiles), &QObject::deleteLater);
    QSharedPointer<GridTileSource> gridTiles(new GridTileSource(), &QObject::deleteLater);
    QSharedPointer<CompositeTileSource> composite(new CompositeTileSource(), &QObject::deleteLater);
    composite->addSourceBottom(osmTiles);
    composite->addSourceBottom(aerialTiles);
    composite->addSourceTop(gridTiles);
    view->setTileSource(composite);

    //Create a widget in the dock that lets us configure tile source layers
    CompositeTileSourceConfigurationWidget * tileConfigWidget = new CompositeTileSourceConfigurationWidget(composite.toWeakRef(),
                                                                                         this->ui->dockWidget);
    this->ui->dockWidget->setWidget(tileConfigWidget);
    this->ui->dockWidget->setVisible(false);
    delete this->ui->dockWidgetContents;

    _tb = new toolbox();

    this->ui->dockWidget3->setWidget(_tb);
    this->ui->centralWidget->setVisible(false);



    this->ui->menuWindow->addAction(this->ui->dockWidget->toggleViewAction());

    this->ui->menuWindow->addAction(this->ui->dockWidget3->toggleViewAction());
    this->ui->dockWidget->toggleViewAction()->setText("&Layers");
    this->ui->dockWidget->toggleViewAction()->setText("&Data");
    this->ui->dockWidget3->toggleViewAction()->setText("&Toolbox");

    QObject::connect(view,SIGNAL(map_clicked(QPointF)),this,SLOT(mapClick(QPointF)));
    QObject::connect(view,SIGNAL(mouse_moved(QPointF)),this,SLOT(getMouseCoord(QPointF)));
    QObject::connect(view,SIGNAL(zoomLevelChanged(quint8)),this,SLOT(setMapItems(quint8)));
    QObject::connect(view,SIGNAL(zoomLevelChanged(quint8)),this,SLOT(newAPRSquery(quint8)));


    QObject::connect(_tb->ui->addMobileButton,SIGNAL(clicked()),this,SLOT(setMobileType()));
    QObject::connect(_tb->ui->addMobileButton,SIGNAL(clicked()),this,SLOT(showEditBoxes()));
    QObject::connect(_tb->ui->addGroundButton,SIGNAL(clicked()),this,SLOT(setGroundType()));
    QObject::connect(_tb->ui->addGroundButton,SIGNAL(clicked()),this,SLOT(showEditBoxes()));
    QObject::connect(_tb->ui->addFPButton,SIGNAL(clicked()),this,SLOT(setFPType()));
    QObject::connect(_tb->ui->addFPButton,SIGNAL(clicked()),this,SLOT(showEditBoxes()));
    QObject::connect(_tb->ui->clearLeftButton,SIGNAL(clicked()),this,SLOT(clearLeftDocks()));

    QObject::connect(_tb->ui->startFlightgearButton,SIGNAL(clicked()),this,SLOT(startFGFS()));
    QObject::connect(_tb->ui->connectTelnetButton,SIGNAL(clicked()),this->_telnet,SLOT(connectToFGFS()));

    QObject::connect(_tb->ui->sendToFlightgearButton,SIGNAL(clicked()),this,SLOT(sendFlightgearData()));
    QObject::connect(_tb->ui->startUpdateButton,SIGNAL(clicked()),this,SLOT(startSignalUpdate()));

    /*\ This needs to go
    QPolygonF polygon;
    polygon << QPointF(10.4, 20.5) << QPointF(20.2, 30.2) << QPointF(24.2, 45.2);
    PolygonObject * obj = new PolygonObject(polygon, QColor(20,200,20,200));

    scene->addObject(obj);
    */

    this->restoreMapState();
    view->setZoomLevel(4);
    view->centerOn(24.658752, 46.255456);
    view->_childView->viewport()->setCursor(Qt::ArrowCursor);
    /* THis shows some pretty radar images, we are not using
    WeatherManager * weatherMan = new WeatherManager(scene, this);
    Q_UNUSED(weatherMan)
    */
}

MainWindow::~MainWindow()
{
    delete ui;
    delete _telnet;
    delete _db;
    delete _remote;
}

//private slot
void MainWindow::on_actionExit_triggered()
{
    this->close();
}

void MainWindow::connectToAPRS()
{
    QVector<FlightgearPrefs *> prefs = _db->select_prefs();
    FlightgearPrefs *p;
    QString aprs_server;
    if(prefs.size()>0)
    {
         p = prefs[0];
         aprs_server = p->_aprs_server;
    }
    else
    {
        aprs_server = "rotate.aprs.net";
    }
    _aprs = new Aprs(aprs_server);
    QObject::connect(_aprs,SIGNAL(aprsData(AprsStation*)),this,SLOT(processAPRSData(AprsStation*)));
}

void MainWindow::newAPRSquery(quint8 zoom)
{
    QPointF cursor_pos = _view->_childView->mapToScene(_view->_childView->mapFromGlobal(QCursor::pos()));

    QPointF pos = Util::convertToLL(cursor_pos, zoom);
    _aprs->setFilter(pos);
}

void MainWindow::processAPRSData(AprsStation *st)
{
    QPointF pos(st->longitude,st->latitude);
    double zoom = _view->zoomLevel();
    QString filename = ":aprs/aprs_icons/slice_";
    QString icon = st->getImage();
    filename.append(icon).append(".png");
    QPixmap pixmap(filename);
    pixmap = pixmap.scaled(16,16);
    QGraphicsPixmapItem *img= _view->_childView->scene()->addPixmap(pixmap);
    QPointF xypos = Util::convertToXY(pos, zoom);
    img->setOffset(xypos - QPoint(8,8));
    _map_aprs.insert(img, pos);


    QGraphicsTextItem * callsign = new QGraphicsTextItem;
    callsign->setPos(xypos - QPoint(0,16));
    callsign->setPlainText(st->callsign);

    _view->_childView->scene()->addItem(callsign);
    _map_aprs_text.insert(callsign,pos);
    delete st;
}

void MainWindow::startFGFS()
{
    Util::startFlightgear(_db);
}

void MainWindow::connectionSuccess()
{
    ConnectionSuccessDialog *dialog = new ConnectionSuccessDialog;
    this->_tb->ui->connectTelnetButton->setEnabled(false);
    dialog->show();
    this->ui->dockWidget3->toggleViewAction()->setText("&Toolbox (active)");
}

void MainWindow::connectionFailure()
{
    ConnectionSuccessDialog *dialog = new ConnectionSuccessDialog;
    this->_tb->ui->connectTelnetButton->setEnabled(true);
    dialog->ui->label->setText("Could not connect to Flightgear. Maybe it's not running?");
    dialog->show();
    this->ui->dockWidget3->toggleViewAction()->setText("&Toolbox (active)");
}


void MainWindow::showSettingsDialog()
{
    SettingsDialog *dialog = new SettingsDialog(_db);

    dialog->show();

}

void MainWindow::getMouseCoord(QPointF coord)
{
    double zoom = _view->zoomLevel();
    QPointF newpos = Util::convertToLL(coord,zoom);
    _tb->ui->label_lat->setText(QString::number(newpos.rx()));
    _tb->ui->label_lon->setText(QString::number(newpos.ry()));
}

void MainWindow::mapClick(QPointF pos)
{
    double zoom = _view->zoomLevel();
    QPointF newpos = Util::convertToLL(pos,zoom);
    QString lon;
    QString lat;
    QDateTime dt = QDateTime::currentDateTime();
    unsigned time = dt.toTime_t();
    switch(_placed_item_type)
    {
    case 1:
        _remote->set_mobile(0);


        _tb->ui->label_lat->setText(lat.setNum(newpos.rx()));
        _tb->ui->label_lon->setText(lon.setNum(newpos.ry()));
    {
        if(_map_mobiles.size() > 0)
        {
            QMap<QGraphicsPixmapItem *, QPointF>::const_iterator it = _map_mobiles.begin();
            QGraphicsPixmapItem * oldicon = it.key();
            _map_mobiles.remove(oldicon);
            _view->_childView->scene()->removeItem(oldicon);
        }
        QPixmap pixmap(":icons/images/phone.png");
        pixmap = pixmap.scaled(32,32);
        QGraphicsPixmapItem *phone= _view->_childView->scene()->addPixmap(pixmap);
        QPointF phone_pos = _view->_childView->mapToScene(_view->_childView->mapFromGlobal(QCursor::pos()-QPoint(16,16)));
        phone->setOffset(phone_pos);
        _map_mobiles.insert(phone, newpos);
        _db->add_mobile_station(0,newpos.rx(),newpos.ry(),time);
    }

        break;
    case 2:

        if(_map_ground.size() > 3)
            break;

        _tb->ui->label_lat->setText(lat.setNum(newpos.rx()));
        _tb->ui->label_lon->setText(lon.setNum(newpos.ry()));
    {
        QPixmap pixmap(":icons/images/antenna.png");
        pixmap = pixmap.scaled(32,32);
        QGraphicsPixmapItem *antenna= _view->_childView->scene()->addPixmap(pixmap);
        QPointF antenna_pos = _view->_childView->mapToScene(_view->_childView->mapFromGlobal(QCursor::pos()-QPoint(16,16)));
        antenna->setOffset(antenna_pos);
        _map_ground.insert(antenna, newpos);
        _db->add_ground_station(0,newpos.rx(),newpos.ry(),time);
    }
        break;
    case 3:

        _tb->ui->label_lat->setText(lat.setNum(newpos.rx()));
        _tb->ui->label_lon->setText(lon.setNum(newpos.ry()));
    {
        QPixmap pixmap(":icons/images/flag.png");
        pixmap = pixmap.scaled(32,32);
        QGraphicsPixmapItem *fppos= _view->_childView->scene()->addPixmap(pixmap);
        QPointF fppos_pos = _view->_childView->mapToScene(_view->_childView->mapFromGlobal(QCursor::pos()-QPoint(7,25)));
        fppos->setOffset(fppos_pos);
        _map_fppos.insert(fppos, newpos);
        _db->add_flightplan_position(0,newpos.rx(),newpos.ry(),time);
    }
        break;
    default:
        qDebug("unknown op");
        break;

    }
    this->showEditBoxes();

}


void MainWindow::setMapItems(quint8 zoom)
{
    {
        QMapIterator<QGraphicsPixmapItem *, QPointF> i(_map_mobiles);
        while (i.hasNext()) {
            i.next();
            QPointF pos = i.value();
            QPointF xypos = Util::convertToXY(pos, zoom);
            QGraphicsPixmapItem * img = i.key();
            img->setOffset(xypos - QPoint(16,16));

        }
    }

    {

        QMapIterator<QGraphicsPixmapItem *, QPointF> i(_map_ground);
        while (i.hasNext()) {
            i.next();
            QPointF pos = i.value();
            QPointF xypos = Util::convertToXY(pos, zoom);
            QGraphicsPixmapItem * img = i.key();
            img->setOffset(xypos - QPoint(16,16));

        }
    }

    {

        QMapIterator<QGraphicsPixmapItem *, QPointF> i(_map_fppos);
        while (i.hasNext()) {
            i.next();
            QPointF pos = i.value();
            QPointF xypos = Util::convertToXY(pos, zoom);
            QGraphicsPixmapItem * img = i.key();
            img->setOffset(xypos - QPoint(7,25));

        }
    }

    {
        QMapIterator<QGraphicsPixmapItem *, QPointF> i(_map_aprs);
        while (i.hasNext()) {
            i.next();
            QPointF pos = i.value();
            QPointF xypos = Util::convertToXY(pos, zoom);
            QGraphicsPixmapItem * img = i.key();
            img->setOffset(xypos - QPoint(8,8));

        }
    }

    {
        QMapIterator<QGraphicsTextItem *, QPointF> i(_map_aprs_text);
        while (i.hasNext()) {
            i.next();
            QPointF pos = i.value();
            QPointF xypos = Util::convertToXY(pos, zoom);
            QGraphicsTextItem * callsign = i.key();
            callsign->setPos(xypos - QPoint(0,16));

        }
    }

}

void MainWindow::setMobileType()
{
    _placed_item_type = 1;
}

void MainWindow::setGroundType()
{
    _placed_item_type = 2;
}

void MainWindow::setFPType()
{
    _placed_item_type = 3;
}


void MainWindow::restoreMapState()
{
    // mobile
    QVector<MobileStation *> mobiles = _db->select_mobile_station(0);
    MobileStation *mobile = mobiles[0];
    QPixmap pixmap(":icons/images/phone.png");
    pixmap = pixmap.scaled(32,32);
    QGraphicsPixmapItem *phone= _view->_childView->scene()->addPixmap(pixmap);
    QPointF pos = QPointF(mobile->longitude,mobile->latitude);
    int zoom = _view->zoomLevel();
    QPointF xypos = Util::convertToXY(pos, zoom);
    phone->setOffset(xypos - QPoint(16,16));
    _map_mobiles.insert(phone, pos);
    delete mobile;
    mobiles.clear();

    // ground
    QVector<GroundStation *> ground_stations = _db->select_ground_stations(0);
    for (int i=0;i<ground_stations.size();++i)
    {
        GroundStation *gs = ground_stations.at(i);
        QPixmap pixmap(":icons/images/antenna.png");
        pixmap = pixmap.scaled(32,32);
        QGraphicsPixmapItem *antenna= _view->_childView->scene()->addPixmap(pixmap);
        QPointF pos = QPointF(gs->longitude,gs->latitude);
        int zoom = _view->zoomLevel();
        QPointF xypos = Util::convertToXY(pos, zoom);
        antenna->setOffset(xypos - QPoint(16,16));
        _map_ground.insert(antenna, pos);
        delete gs;
    }
    ground_stations.clear();


    // fp
    QVector<FlightPlanPoints *> fp_points = _db->select_flightplan_positions(0);
    for (int i=0;i<fp_points.size();++i)
    {
        FlightPlanPoints *fp = fp_points.at(i);
        QPixmap pixmap(":icons/images/flag.png");
        pixmap = pixmap.scaled(32,32);
        QGraphicsPixmapItem *flag= _view->_childView->scene()->addPixmap(pixmap);
        QPointF pos = QPointF(fp->longitude,fp->latitude);
        int zoom = _view->zoomLevel();
        QPointF xypos = Util::convertToXY(pos, zoom);
        flag->setOffset(xypos - QPoint(7,25));
        _map_fppos.insert(flag, pos);
        delete fp;
    }
    fp_points.clear();

}


void MainWindow::clearLeftDocks()
{
    for (int j=0;j<_docks.size();++j)
    {
        this->removeDockWidget(_docks.at(j));
        delete _docks.at(j);
    }
    _docks.clear();
    _show_signals =false;
    _station_ids.clear();
}

void MainWindow::showEditBoxes()
{

    switch(_placed_item_type)
    {
    case 1:
    {
        //mobile
        _show_signals =false;
        _station_ids.clear();
        for (int j=0;j<_docks.size();++j)
        {
            this->removeDockWidget(_docks.at(j));
            delete _docks.at(j);
        }
        _docks.clear();
        QVector<MobileStation *> mobiles = _db->select_mobile_station(0);
        MobileStation *mobile = mobiles[0];
        MobileForm *mf = new MobileForm;
        //this->ui->dockWidget2->setWidget(mf);
        mf->ui->idEdit->setText(QString::number(mobile->id));
        mf->ui->lonEdit->setText(QString::number(mobile->longitude));
        mf->ui->latEdit->setText(QString::number(mobile->latitude));
        mf->ui->nameEdit->setText(mobile->name);
        mf->ui->frequencyEdit->setText(QString::number(mobile->frequency));
        mf->ui->headingEdit->setText(QString::number(mobile->heading_deg));
        mf->ui->altitudeEdit->setText(QString::number(mobile->elevation_feet));
        mf->ui->terrainFollowingEdit->setText(QString::number(mobile->terrain_following));
        mf->ui->speedEdit->setText(QString::number(mobile->speed));
        QObject::connect(mf,SIGNAL(haveData(MobileStation*)),this,SLOT(saveMobile(MobileStation *)));
        QDockWidget *dw = new QDockWidget;
        dw->setWindowTitle(mobile->name+" "+QString::number( mobile->id));
        dw->setMaximumWidth(260);
        dw->setWidget(mf);
        this->addDockWidget(Qt::LeftDockWidgetArea,dw);
        _docks.push_back(dw);
        //mf->show();
        delete mobile;
        mobiles.clear();
    }
        break;

    case 2:
    {
        //ground
        _show_signals =false;
        _station_ids.clear();
        for (int j=0;j<_docks.size();++j)
        {
            this->removeDockWidget(_docks.at(j));
            delete _docks.at(j);
        }
        _docks.clear();
        QVector<GroundStation *> ground_stations = _db->select_ground_stations(0);

        for (int i=0;i<ground_stations.size();++i)
        {
            GroundStation *gs = ground_stations.at(i);
            GroundStationForm *gs_form = new GroundStationForm;
            gs_form->ui->idEdit->setText(QString::number(gs->id));
            gs_form->ui->nameEdit->setText(gs->name);
            gs_form->ui->headingDegLineEdit->setText(QString::number(gs->heading_deg));
            gs_form->ui->pitchDegLineEdit->setText(QString::number(gs->pitch_deg));
            gs_form->ui->elevationFeetLineEdit->setText(QString::number(gs->elevation_feet));
            gs_form->ui->frequencyEdit->setText(QString::number(gs->frequency));
            gs_form->ui->beaconDelayLineEdit->setText(QString::number(gs->beacon_delay));
            gs_form->ui->transmissionTypeLineEdit->setText(QString::number(gs->transmission_type));
            gs_form->ui->polarizationLineEdit->setText(QString::number(gs->polarization));
            gs_form->ui->rxAntennaHeightLineEdit->setText(QString::number(gs->rx_antenna_height));
            gs_form->ui->rxAntennaGainLineEdit->setText(QString::number(gs->rx_antenna_gain));
            gs_form->ui->rxAntennaTypeLineEdit->setText(gs->rx_antenna_type);
            gs_form->ui->rxLineLossesLineEdit->setText(QString::number(gs->rx_line_losses));
            gs_form->ui->rxSensitivityLineEdit->setText(QString::number(gs->rx_sensitivity));
            gs_form->ui->txPowerWattLineEdit->setText(QString::number(gs->tx_power_watt));
            gs_form->ui->txAntennaHeightLineEdit->setText(QString::number(gs->tx_antenna_height));
            gs_form->ui->txAntennaGainLineEdit->setText(QString::number(gs->tx_antenna_gain));
            gs_form->ui->txAntennaTypeLineEdit->setText(gs->tx_antenna_type);
            gs_form->ui->txLineLossesLineEdit->setText(QString::number(gs->tx_line_losses));

            QDockWidget *dw = new QDockWidget;
            dw->setWindowTitle(QString::number( gs->id));
            dw->setMaximumWidth(260);
            dw->setWidget(gs_form);
            this->addDockWidget(Qt::LeftDockWidgetArea,dw);
            _docks.push_back(dw);
            //this->ui->dockWidget2->setWidget(gs_form);
            QObject::connect(gs_form,SIGNAL(haveData(GroundStation*)),this,SLOT(saveGroundStation(GroundStation *)));
            QObject::connect(gs_form,SIGNAL(delStation(unsigned)),this,SLOT(deleteGroundStation(unsigned)));
            //gs_form->show();
            delete gs;
        }
        for (int j=0;j<_docks.size();++j)
        {
            if((j+1)==_docks.size()) continue;
            this->tabifyDockWidget(_docks.at(j),_docks.at(j+1));
        }
        ground_stations.clear();
    }

        break;

    case 3:
    {
        //fp pos
        _show_signals =false;
        _station_ids.clear();
        for (int j=0;j<_docks.size();++j)
        {
            this->removeDockWidget(_docks.at(j));
            delete _docks.at(j);
        }
        _docks.clear();
        QVector<FlightPlanPoints *> fp_points = _db->select_flightplan_positions(0);
        for (int i=0;i<fp_points.size();++i)
        {
            FlightPlanPoints *fp = fp_points.at(i);
            FlightplanForm *fp_form = new FlightplanForm;
            fp_form->ui->idLineEdit->setText(QString::number(fp->id));
            fp_form->ui->lonLineEdit->setText(QString::number(fp->longitude));
            fp_form->ui->latLineEdit->setText(QString::number(fp->latitude));
            fp_form->ui->altitudeLineEdit->setText(QString::number(fp->altitude));
            //this->ui->dockWidget2->setWidget(gs_form);
            QObject::connect(fp_form,SIGNAL(haveData(FlightPlanPoints *)),this,SLOT(saveFlightplan(FlightPlanPoints*)));
            QObject::connect(fp_form,SIGNAL(delFP(unsigned)),this,SLOT(deleteFlightplan(unsigned)));
            QDockWidget *dw = new QDockWidget;
            dw->setWindowTitle(QString::number( fp->id));
            dw->setMaximumWidth(260);
            dw->setWidget(fp_form);
            this->addDockWidget(Qt::LeftDockWidgetArea,dw);
            _docks.push_back(dw);
            //fp_form->show();
            delete fp;
        }
        for (int j=0;j<_docks.size();++j)
        {
            if((j+1)==_docks.size()) continue;
            this->tabifyDockWidget(_docks.at(j),_docks.at(j+1));
        }
        fp_points.clear();
    }
        break;
    }
}

void MainWindow::saveMobile(MobileStation * m)
{
    _db->update_mobile_station(0, m->id, m->name, m->frequency,
                               m->elevation_feet, m->heading_deg,
                               m->tx_power_watt, m->terrain_following,
                               m->speed,0);
    delete m;

}

void MainWindow::saveGroundStation(GroundStation * g)
{
    _db->update_ground_station(0,g->id,g->name,g->frequency,g->beacon_delay,
                               g->transmission_type,g->elevation_feet,g->heading_deg,
                               g->pitch_deg,g->polarization,g->tx_antenna_height,g->tx_antenna_type,
                               g->tx_antenna_gain,g->tx_line_losses,g->tx_power_watt,
                               g->rx_antenna_height,g->rx_antenna_type,g->rx_antenna_gain,
                               g->rx_line_losses,g->rx_sensitivity,g->created_on);
    delete g;

}


void MainWindow::saveFlightplan(FlightPlanPoints * fp)
{
    _db->update_flightplan_position(fp->altitude,0,fp->id);
    delete fp;

}

void MainWindow::deleteGroundStation(unsigned id)
{
    QPointF dbpos;
    QVector<GroundStation *> ground_stations = _db->select_ground_stations(0);
    for(int i=0;i<ground_stations.size();++i)
    {
        GroundStation *g = ground_stations.at(i);
        if(g->id == id)
        {
            dbpos.setX(g->longitude);
            dbpos.setY(g->latitude);
            break;
        }
    }
    ground_stations.clear();

    QMapIterator<QGraphicsPixmapItem *, QPointF> i(_map_ground);
    while (i.hasNext())
    {
        i.next();
        QPointF pos = i.value();
        QGraphicsPixmapItem *antenna = i.key();
        if((fabs(pos.rx() - dbpos.rx()) <= 0.0001) && (fabs(pos.ry() - dbpos.ry()) <= 0.0001))
        {
            antenna->setOffset(0,0);
            _map_ground.remove(antenna);
        }

    }

    /** experimental*/
    for (int j=0;j<_docks.size();++j)
    {
        if(_docks.at(j)->windowTitle().toInt() == id)
        {
            this->removeDockWidget(_docks.at(j));
            QWidget *widget = _docks.at(j)->widget();
            delete widget;
            delete _docks.at(j);
            _docks.remove(j);
        }
    }


    _db->delete_ground_station(0,id);
}

void MainWindow::deleteFlightplan(unsigned id)
{
    QPointF dbpos;
    QVector<FlightPlanPoints *> fp_points = _db->select_flightplan_positions(0);
    for(int i=0;i<fp_points.size();++i)
    {
        FlightPlanPoints *f = fp_points.at(i);
        if(f->id == id)
        {
            dbpos.setX(f->longitude);
            dbpos.setY(f->latitude);
            break;
        }
    }
    fp_points.clear();

    QMapIterator<QGraphicsPixmapItem *, QPointF> i(_map_fppos);
    while (i.hasNext())
    {
        i.next();
        QPointF pos = i.value();
        QGraphicsPixmapItem *flag = i.key();
        if((fabs(pos.rx() - dbpos.rx()) <= 0.0001) && (fabs(pos.ry() - dbpos.ry()) <= 0.0001))
        {
            flag->setOffset(0,0);
            _map_fppos.remove(flag);
        }

    }
    /** experimental*/
    for (int j=0;j<_docks.size();++j)
    {
        if(_docks.at(j)->windowTitle().toInt() == id)
        {
            this->removeDockWidget(_docks.at(j));
            QWidget *widget = _docks.at(j)->widget();
            delete widget;
            delete _docks.at(j);
            _docks.remove(j);
        }
    }


    _db->delete_flightplan_position(0,id);
}

void MainWindow::sendFlightgearData()
{
    connect(_remote, SIGNAL(readyUpdate()), this, SLOT(startSignalUpdate()));
    _remote->sendAllData();
}

void MainWindow::startSignalUpdate()
{
    _start_time= QDateTime::currentDateTime().toString("d/MMM/yyyy hh:mm:ss");
    QThread *t= new QThread;
    Updater *up = new Updater(_db);
    up->moveToThread(t);
    connect(up, SIGNAL(haveMobilePosition(double,double)), this, SLOT(moveMobile(double,double)));
    connect(up, SIGNAL(haveSignalReading(double, double, uint,QString,double,Signal*)), this, SLOT(showSignalReading(double, double, uint,QString,double,Signal*)));
    //connect(up, SIGNAL(error(QString)), this, SLOT(errorString(QString)));
    connect(t, SIGNAL(started()), up, SLOT(startUpdate()));
    connect(up, SIGNAL(finished()), t, SLOT(quit()));
    connect(up, SIGNAL(finished()), up, SLOT(deleteLater()));
    connect(t, SIGNAL(finished()), t, SLOT(deleteLater()));
    QObject::connect(_telnet,SIGNAL(haveProperty(QString)),this,SLOT(setReceived(QString)),Qt::QueuedConnection);
    QObject::connect(up,SIGNAL(getProperty(QString)),_telnet,SLOT(getProperty(QString)),Qt::QueuedConnection);
    //QObject::connect(this,SIGNAL(haveProperty(QString)),up,SLOT(setReceived(QString)),Qt::QueuedConnection);
    t->start();
    _updater = up;


}

void MainWindow::setReceived(QString data)
{
    //qDebug() << "main received: " << data;
    //emit haveProperty(data);
    _updater->setReceived(data);
}

void MainWindow::moveMobile(double lon, double lat)
{
    if(_map_mobiles.size() > 0)
    {
        QMap<QGraphicsPixmapItem *, QPointF>::const_iterator it = _map_mobiles.begin();
        QPointF pos = it.value();
        if( (fabs(pos.rx()-lon)> 0.1) || ( fabs(pos.ry()-lat)>0.1) )
        {
            return;
        }
        QGraphicsPixmapItem * oldicon = it.key();
        _view->_childView->scene()->removeItem(oldicon);
        _map_mobiles.remove(oldicon);
    }
    QPixmap pixmap(":icons/images/phone.png");
    pixmap = pixmap.scaled(32,32);
    QGraphicsPixmapItem *phone= _view->_childView->scene()->addPixmap(pixmap);

    QPointF pos = QPointF(lon,lat);
    int zoom = _view->zoomLevel();
    QPointF xypos = Util::convertToXY(pos, zoom);
    phone->setOffset(xypos - QPoint(16,16));
    _map_mobiles.insert(phone, pos);
    _db->update_mobile_position(0,lon,lat);

}


void MainWindow::showSignalReading(double lon,double lat,uint id_station,QString station_name,double freq,Signal*s)
{
    for (int j=0;j<_signal_lines.size();++j)
    {
        _view->_childView->scene()->removeItem(_signal_lines.at(j));
        delete _signal_lines.at(j);
    }
    _signal_lines.clear();

    this->_tb->ui->clearLeftButton->setEnabled(false);

    _db->update_signals(id_station,0,s);
    _db->update_replays(id_station,0,lon,lat,_start_time,s);


    if(_show_signals == false)
    {
        for (int j=0;j<_docks.size();++j)
        {
            this->removeDockWidget(_docks.at(j));
            delete _docks.at(j);
        }
        _docks.clear();
    }
    _show_signals =true;
    QVector<GroundStation *> ground_stations = _db->select_ground_stations(0);

    for (int i=0;i<ground_stations.size();++i)
    {

        GroundStation *gs = ground_stations.at(i);
        if(id_station == gs->id)
        {
            if(s->signal >0)
            {
                if(_map_mobiles.size() > 0)
                {
                    QMap<QGraphicsPixmapItem *, QPointF>::const_iterator it = _map_mobiles.begin();
                    QPointF pos = it.value();
                    if( (fabs(pos.rx()-lon)> 0.1) || ( fabs(pos.ry()-lat)>0.1) )
                    {
                        continue;
                    }
                }
                QPointF gs_pos(gs->longitude,gs->latitude);
                QPointF mobile_pos(lon,lat);
                QPointF xy_gs_pos = Util::convertToXY(gs_pos,_view->zoomLevel());
                QPointF xy_mobile_pos = Util::convertToXY(mobile_pos,_view->zoomLevel());
                QLineF signal_line(xy_gs_pos,xy_mobile_pos);
                QBrush brush;
                if(s->signal >0 && s->signal <=10)
                {
                    brush= Qt::yellow;
                }
                else if(s->signal > 10 && s->signal <=30)
                {
                    brush = Qt::green;
                }
                else if(s->signal > 30)
                {
                    brush = Qt::red;
                }
                QPen pen(brush, 3, Qt::DashDotLine, Qt::RoundCap, Qt::RoundJoin);
                QGraphicsLineItem *line1 = _view->_childView->scene()->addLine(signal_line,pen);
                _signal_lines.push_back(line1);
            }

            bool has_been =false;
            for(int k=0;k < _station_ids.size();++k)
            {
                unsigned id = _station_ids.at(k);
                if(id == id_station)
                {
                    has_been =true;
                }
            }
            if((_last_station_id != id_station) && !has_been)
            {
                StationSignalForm *signal_form = new StationSignalForm;
                signal_form->ui->stationName->setText(station_name);
                signal_form->ui->frequency->setText(QString::number(freq));
                signal_form->ui->signal->setText(QString::number(s->signal));
                signal_form->ui->signalDbm->setText(QString::number(s->signal_dbm));
                signal_form->ui->fieldStrength->setText(QString::number(s->field_strength_uv));
                signal_form->ui->linkBudget->setText(QString::number(s->link_budget));
                signal_form->ui->propMode->setText(s->prop_mode);

                double img_width=s->signal / 3 *12;
                if(img_width <= 0) img_width=0;
                if(img_width > 191) img_width=191;
                signal_form->ui->labelSmeter->setMaximumWidth(img_width);
                signal_form->ui->labelSmeter->setMinimumWidth(img_width);



                QDockWidget *dw = new QDockWidget;
                dw->setWindowTitle(QString::number( gs->id));
                dw->setMaximumWidth(260);
                dw->setWidget(signal_form);
                this->addDockWidget(Qt::LeftDockWidgetArea,dw);
                _docks.push_back(dw);
                for (int j=0;j<_docks.size();++j)
                {
                    if((j+1)==_docks.size()) continue;
                    this->tabifyDockWidget(_docks.at(j),_docks.at(j+1));
                }
                _station_ids.push_back(id_station);
            }
            else
            {
                for (int j=0;j<_docks.size();++j)
                {

                    QDockWidget *dw = _docks.at(j);
                    if(dw->windowTitle().toInt() == id_station)
                    {
                        QWidget *w = dw->widget();

                        QLabel * stationName = w->findChild<QLabel *>("stationName");
                        QLabel * frequency = w->findChild<QLabel *>("frequency");
                        QLabel * signal = w->findChild<QLabel *>("signal");
                        QLabel * signalDbm = w->findChild<QLabel *>("signalDbm");
                        QLabel * fieldStrength = w->findChild<QLabel *>("fieldStrength");
                        QLabel * linkBudget = w->findChild<QLabel *>("linkBudget");
                        QLabel * propMode = w->findChild<QLabel *>("propMode");
                        QLabel * labelSmeter = w->findChild<QLabel *>("labelSmeter");

                        stationName->setText(station_name);
                        frequency->setText(QString::number(freq));
                        if(s->signal <= 2)
                        {
                            signal->setText("<font color=\"red\">"+QString::number(s->signal)+"</font>");
                        }
                        else if((s->signal < 10) && (s->signal >2) )
                        {
                            signal->setText("<font color=\"yellow\">"+QString::number(s->signal)+"</font>");
                        }
                        else
                        {
                            signal->setText("<font color=\"green\">"+QString::number(s->signal)+"</font>");
                        }
                        signalDbm->setText(QString::number(s->signal_dbm));
                        fieldStrength->setText(QString::number(s->field_strength_uv));
                        linkBudget->setText(QString::number(s->link_budget));
                        propMode->setText(s->prop_mode);
                        double img_width=s->signal / 3 *12;
                        if(img_width <= 0) img_width=0;
                        if(img_width > 191) img_width=191;
                        labelSmeter->setMaximumWidth(img_width);
                        labelSmeter->setMinimumWidth(img_width);
                    }
                }

            }
            _last_station_id = id_station;

        }
        delete gs;
    }

    ground_stations.clear();

    delete s;
}
