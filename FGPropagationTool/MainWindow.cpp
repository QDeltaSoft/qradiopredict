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

    ui->setupUi(this);
    //!!!!!!!! connections must always come after setupUi!!!
    QObject::connect(ui->actionConnect_to_Flightgear,SIGNAL(triggered()),this->_telnet,SLOT(connectToFGFS()));
    QObject::connect(ui->actionStart_Flightgear,SIGNAL(triggered()),this,SLOT(startFGFS()));
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
    QObject::connect(view,SIGNAL(zoomLevelChanged(quint8)),this,SLOT(setMapItems(quint8)));


    QObject::connect(_tb->ui->addMobileButton,SIGNAL(clicked()),this,SLOT(setMobileType()));
    QObject::connect(_tb->ui->addMobileButton,SIGNAL(clicked()),this,SLOT(showEditBoxes()));
    QObject::connect(_tb->ui->addGroundButton,SIGNAL(clicked()),this,SLOT(setGroundType()));
    QObject::connect(_tb->ui->addGroundButton,SIGNAL(clicked()),this,SLOT(showEditBoxes()));
    QObject::connect(_tb->ui->addFPButton,SIGNAL(clicked()),this,SLOT(setFPType()));
    QObject::connect(_tb->ui->addFPButton,SIGNAL(clicked()),this,SLOT(showEditBoxes()));

    QObject::connect(_tb->ui->startFlightgearButton,SIGNAL(clicked()),this,SLOT(startFGFS()));
    QObject::connect(_tb->ui->connectTelnetButton,SIGNAL(clicked()),this->_telnet,SLOT(connectToFGFS()));

    QObject::connect(_tb->ui->sendToFlightgearButton,SIGNAL(clicked()),this,SLOT(sendFlightgearData()));
    QObject::connect(_tb->ui->startUpdateButton,SIGNAL(clicked()),this,SLOT(startUpdate()));

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
    WeatherManager * weatherMan = new WeatherManager(scene, this);
    Q_UNUSED(weatherMan)
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

void MainWindow::startFGFS()
{
    Util::startFlightgear();
}

void MainWindow::connectionSuccess()
{
    ConnectionSuccessDialog *dialog = new ConnectionSuccessDialog;

    dialog->show();
    this->ui->dockWidget3->toggleViewAction()->setText("&Toolbox (active)");
}

void MainWindow::connectionFailure()
{
    ConnectionSuccessDialog *dialog = new ConnectionSuccessDialog;
    dialog->ui->label->setText("Could not connect to Flightgear. Maybe it's not running?");
    dialog->show();
    this->ui->dockWidget3->toggleViewAction()->setText("&Toolbox (active)");
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
        //_remote->set_ground(newpos);
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
        //_remote->set_fp(newpos);

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

void MainWindow::showEditBoxes()
{

    switch(_placed_item_type)
    {
    case 1:
    {
        //mobile
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

    /** experimental
    for (int j=0;j<_docks.size();++j)
    {
        if(_docks.at(j)->windowTitle().toInt() == id)
        {
            this->removeDockWidget(_docks.at(j));
            QWidget *widget = _docks.at(j)->widget();
            delete widget;
            delete _docks.at(j);
        }
    }
    */

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
    /** experimental
    for (int j=0;j<_docks.size();++j)
    {
        if(_docks.at(j)->windowTitle().toInt() == id)
        {
            this->removeDockWidget(_docks.at(j));
            QWidget *widget = _docks.at(j)->widget();
            delete widget;
            delete _docks.at(j);
        }
    }
    */

    _db->delete_flightplan_position(0,id);
}

void MainWindow::sendFlightgearData()
{
    _remote->sendAllData();
}

void MainWindow::startUpdate()
{
    QThread *t= new QThread;
    Updater *up = new Updater(_telnet, _db);
    up->moveToThread(t);
    connect(up, SIGNAL(haveMobilePosition(double,double)), this, SLOT(moveMobile(double,double)));
    connect(up, SIGNAL(error(QString)), this, SLOT(errorString(QString)));
    connect(t, SIGNAL(started()), up, SLOT(startUpdate()));
    connect(up, SIGNAL(finished()), t, SLOT(quit()));
    connect(up, SIGNAL(finished()), up, SLOT(deleteLater()));
    connect(t, SIGNAL(finished()), t, SLOT(deleteLater()));
    t->start();
}

void MainWindow::moveMobile(double lon, double lat)
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

    QPointF pos = QPointF(lon,lat);
    int zoom = _view->zoomLevel();
    QPointF xypos = Util::convertToXY(pos, zoom);
    phone->setOffset(xypos - QPoint(16,16));
    _map_mobiles.insert(phone, pos);

}
