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
    _remote = new FGRemote(_telnet);

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
    this->ui->dockWidget3->toggleViewAction()->setText("&Toolbox");

    QObject::connect(view,SIGNAL(map_clicked(QPointF)),this,SLOT(mapClick(QPointF)));
    QObject::connect(view,SIGNAL(zoomLevelChanged(quint8)),this,SLOT(setMapItems(quint8)));


    QObject::connect(_tb->ui->addMobileButton,SIGNAL(clicked()),this,SLOT(setMobileType()));
    QObject::connect(_tb->ui->addGroundButton,SIGNAL(clicked()),this,SLOT(setGroundType()));
    QObject::connect(_tb->ui->addFPButton,SIGNAL(clicked()),this,SLOT(setFPType()));

    QPolygonF polygon;
    polygon << QPointF(10.4, 20.5) << QPointF(20.2, 30.2) << QPointF(24.2, 45.2);
    PolygonObject * obj = new PolygonObject(polygon, QColor(20,200,20,200));

    scene->addObject(obj);

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
        _remote->set_mobile(newpos);


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
    qDebug() << mobile->longitude << " " << mobile->latitude << " " << xypos.rx() << " " << xypos.ry();
    phone->setOffset(xypos - QPoint(16,16));
    _map_mobiles.insert(phone, pos);

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
    }


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
    }

}

