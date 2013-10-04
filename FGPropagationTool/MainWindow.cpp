#include "MainWindow.h"
#include "ui_MainWindow.h"




MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    _telnet = new FGTelnet;
    _db = new DatabaseApi;
    _remote = new FGRemote(_telnet, _db);
    _show_signals = false;
    _last_station_id = -1;
    _plot_opacity = 15;
    _last_plot_point.setX(0);
    _last_plot_point.setY(0);
    _plot_pixmap = new QPixmap(1000,1000);
    _plotvalues = new QVector<PlotValue*>;


    //!!!!!!!! connections must always come after setupUi!!!
    QObject::connect(ui->actionConnect_to_Flightgear,SIGNAL(triggered()),this->_telnet,SLOT(connectToFGFS()));
    QObject::connect(ui->actionStart_Flightgear,SIGNAL(triggered()),this,SLOT(startFGFS()));
    QObject::connect(ui->actionSend_to_Flightgear,SIGNAL(triggered()),this,SLOT(sendFlightgearData()));
    QObject::connect(ui->action_Settings,SIGNAL(triggered()),this,SLOT(showSettingsDialog()));
    QObject::connect(ui->actionConnect_APRS,SIGNAL(triggered()),this,SLOT(connectToAPRS()));
    QObject::connect(ui->actionSave_plot,SIGNAL(triggered()),this,SLOT(savePlot()));
    QObject::connect(ui->actionLoad_plot,SIGNAL(triggered()),this,SLOT(loadPlot()));
    QObject::connect(this->_telnet,SIGNAL(connectedToFGFS()),this,SLOT(connectionSuccess()));
    QObject::connect(this->_telnet,SIGNAL(connectionFailure()),this,SLOT(connectionFailure()));


    MapGraphicsScene * scene = new MapGraphicsScene(this);
    MapGraphicsView * view = new MapGraphicsView(scene,this);
    _view=view;
    //view->_childView->setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers | QGL::DirectRendering)));
    //view->_childView->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    //view->_childView->update();

    this->setCentralWidget(view);


    QSharedPointer<OSMTileSource> osmTiles(new OSMTileSource(OSMTileSource::OSMTiles), &QObject::deleteLater);
    QSharedPointer<OSMTileSource> aerialTiles(new OSMTileSource(OSMTileSource::MapQuestAerialTiles), &QObject::deleteLater);
    QSharedPointer<GridTileSource> gridTiles(new GridTileSource(), &QObject::deleteLater);
    QSharedPointer<CompositeTileSource> composite(new CompositeTileSource(), &QObject::deleteLater);
    composite->addSourceBottom(osmTiles);
    composite->addSourceBottom(aerialTiles);
    composite->addSourceTop(gridTiles);
    view->setTileSource(composite);


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
    QObject::connect(_tb->ui->stopUpdateButton,SIGNAL(clicked()),this,SLOT(stopSignalUpdate()));

    QObject::connect(_tb->ui->startStandaloneButton,SIGNAL(clicked()),this,SLOT(startStandalone()));
    QObject::connect(_tb->ui->stopStandaloneButton,SIGNAL(clicked()),this,SLOT(stopStandalone()));

    QObject::connect(_tb->ui->opacitySlider,SIGNAL(valueChanged(int)),this,SLOT(changePlotOpacity(int)));

    QObject::connect(_tb->ui->aprsTimeSpinBox,SIGNAL(valueChanged(int)),this,SLOT(changeAPRSTimeFilter(int)));

    _tb->ui->startFlightgearButton->setVisible(true);
    _tb->ui->connectTelnetButton->setVisible(true);
    _tb->ui->sendToFlightgearButton->setVisible(true);
    _tb->ui->startUpdateButton->setVisible(true);
    _tb->ui->stopUpdateButton->setVisible(true);

    _tb->ui->progressBar->setVisible(false);

    //this->createActions();
    //this->createTrayIcon();

    _painted_pix = _view->_childView->scene()->addPixmap(*_plot_pixmap);
    _painted_pix->setOpacity(0.0);
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
    _raw_aprs_messages.clear();

    /**
    delete _trayIcon;
    delete _trayIconMenu;
    delete _restoreAction;
    delete _quitAction;
    */
}

//private slot
void MainWindow::on_actionExit_triggered()
{
    this->close();
}

void MainWindow::createActions()
 {
     _restoreAction = new QAction(tr("&Restore"), this);
     connect(_restoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));

     _quitAction = new QAction(tr("&Quit"), this);
     connect(_quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
 }

void MainWindow::createTrayIcon()
 {
    _trayIconMenu = new QMenu(this);
    _trayIconMenu->addAction(_restoreAction);
    _trayIconMenu->addSeparator();
    _trayIconMenu->addAction(_quitAction);

    _trayIcon = new QSystemTrayIcon(this);
    _trayIcon->setContextMenu(_trayIconMenu);
    QIcon icon;
    QPixmap px;
    px.load(":icons/images/flag.png");
    icon.addPixmap(px, QIcon::Normal, QIcon::Off);
    _trayIcon->setIcon(icon);
    //setWindowIcon(icon);
    connect(_trayIcon,SIGNAL(messageClicked()),this,SLOT(raise()));

    _trayIcon->setToolTip("Click me");
    _trayIcon->setVisible(true);
    _trayIcon->show();

    _trayIcon->hide();
    qApp->processEvents();
    _trayIcon->show();
}

/** not used right now
void MainWindow::closeEvent(QCloseEvent *event)
{

    if (_trayIcon->isVisible())
    {
        QMessageBox::information(this, tr("Systray"),
                          tr("The program will keep running in the "
                             "system tray. To terminate the program, "
                             "choose <b>Quit</b> in the context menu "
                             "of the system tray entry."));
        hide();
        event->ignore();
    }
}
*/

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
    QObject::connect(_aprs,SIGNAL(rawAprsData(QString)),this,SLOT(processRawAPRSData(QString)));
    QObject::connect(ui->actionRaw_APRS_messages,SIGNAL(triggered()),this,SLOT(showRawAPRSMessages()));
    delete p;
    prefs.clear();
}

void MainWindow::showRawAPRSMessages()
{
    RawMessagesForm *f = new RawMessagesForm;
    for(int i=0; i<_raw_aprs_messages.size();++i)
    {
        f->ui->messagesTextEdit->append(*_raw_aprs_messages[i]);
    }
    QObject::connect(this,SIGNAL(newMessage(QString)),f,SLOT(addMessage(QString)));
    f->show();
}

void MainWindow::changeAPRSTimeFilter(int hours)
{
    QMapIterator<AprsPixmapItem *, AprsIcon> i(_map_aprs);
    while(i.hasNext())
    {
        i.next();
        delete i.key();
    }
    _map_aprs.clear();
    QMapIterator<QGraphicsTextItem *, QPointF> j(_map_aprs_text);
    while(j.hasNext())
    {
        j.next();
        delete j.key();
    }
    _map_aprs_text.clear();

    int time_now = QDateTime::currentDateTime().toTime_t();

    QVector<AprsStation *> filtered_stations = _db->filter_aprs_stations(time_now - (hours*3600));
    for(int k=0;k<filtered_stations.size();++k)
    {
        AprsStation *st = filtered_stations[k];
        QPointF pos(st->longitude,st->latitude);
        double zoom = _view->zoomLevel();
        QString filename = ":aprs/aprs_icons/slice_";
        AprsIcon ic;
        QString icon = st->getImage();
        ic.icon = icon;
        ic.position = pos;
        filename.append(icon).append(".png");
        QPixmap pixmap(filename);
        pixmap = pixmap.scaled(16,16);
        AprsPixmapItem *img = new AprsPixmapItem(pixmap);
        img->setAcceptHoverEvents(true);

        _view->_childView->scene()->addItem(img);
        QPointF xypos = Util::convertToXY(pos, zoom);
        img->setMessage(st->callsign,st->via,st->message);
        img->setPosition(xypos);
        img->setOffset(xypos - QPoint(8,8));
        _map_aprs.insert(img, ic);

        QString callsign_text;
        QRegularExpression re(";(.+?)\\*");
        QRegularExpressionMatch match = re.match(st->payload);
        if(match.hasMatch())
        {
            callsign_text = match.captured(1);
        }
        else
        {
            callsign_text = st->callsign;
        }
        QGraphicsTextItem * callsign = new QGraphicsTextItem;
        callsign->setPos(xypos - QPoint(0,16));
        callsign->setPlainText(callsign_text);

        _view->_childView->scene()->addItem(callsign);
        _map_aprs_text.insert(callsign,pos);
        delete st;
    }

}

void MainWindow::newAPRSquery(quint8 zoom)
{
    QPointF cursor_pos = _view->_childView->mapToScene(_view->_childView->mapFromGlobal(QCursor::pos()));

    QPointF pos = Util::convertToLL(cursor_pos, zoom);
    QVector<FlightgearPrefs *> prefs = _db->select_prefs();
    FlightgearPrefs *settings;
    if(prefs.size()>0)
    {
        settings = prefs[0];
    }
    else
        settings = 0;
    int range = settings->_aprs_filter_range;
    _aprs->setFilter(pos, range);
}

void MainWindow::processRawAPRSData(QString data)
{
    QString * raw_message = new QString(data);
    _raw_aprs_messages.push_back(raw_message);
    emit newMessage(data);
}

void MainWindow::processAPRSData(AprsStation *st)
{

    QMapIterator<AprsPixmapItem*, AprsIcon> i(_map_aprs);
    while(i.hasNext())
    {
        i.next();
        AprsIcon ic = i.value();
        QPointF pos= ic.position;
        if( (fabs(pos.rx() - st->longitude) <= 0.001) && (fabs(pos.ry() - st->latitude) <=0.001 )
                && (ic.icon==st->getImage()) )
        {
            return;
        }
        if((ic.callsign == st->callsign) &&
                (fabs(pos.rx() - st->longitude) > 0.001) &&
                (fabs(pos.ry() - st->latitude) >0.001 ) &&
                (ic.time_seen > (st->time_seen - 3600)))
        {

        }

    }
    QPointF pos(st->longitude,st->latitude);
    double zoom = _view->zoomLevel();
    QString filename = ":aprs/aprs_icons/slice_";
    AprsIcon ic;
    QString icon = st->getImage();
    ic.icon = icon;
    ic.position = pos;
    ic.callsign = st->callsign;
    ic.time_seen = st->time_seen;
    filename.append(icon).append(".png");
    QPixmap pixmap(filename);
    pixmap = pixmap.scaled(16,16);
    AprsPixmapItem *img = new AprsPixmapItem(pixmap);
    img->setAcceptHoverEvents(true);

    _view->_childView->scene()->addItem(img);
    QPointF xypos = Util::convertToXY(pos, zoom);
    img->setMessage(st->callsign,st->via,st->message);
    img->setPosition(xypos);
    img->setOffset(xypos - QPoint(8,8));
    _map_aprs.insert(img, ic);

    QString callsign_text;
    QRegularExpression re(";(.+?)\\*");
    QRegularExpressionMatch match = re.match(st->payload);
    if(match.hasMatch())
    {
        callsign_text = match.captured(1);
    }
    else
    {
        callsign_text = st->callsign;
    }
    QGraphicsTextItem * callsign = new QGraphicsTextItem;
    callsign->setPos(xypos - QPoint(0,16));
    callsign->setPlainText(callsign_text);

    _view->_childView->scene()->addItem(callsign);
    _map_aprs_text.insert(callsign,pos);
    _db->update_aprs_stations(st);
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
        QMapIterator<AprsPixmapItem *, AprsIcon> i(_map_aprs);
        while (i.hasNext()) {
            i.next();
            AprsIcon ic = i.value();
            QPointF pos = ic.position;
            QPointF xypos = Util::convertToXY(pos, zoom);
            AprsPixmapItem * img = i.key();
            img->setOffset(xypos - QPoint(8,8));
            img->setPosition(xypos);

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

    {
        QMapIterator<QGraphicsPolygonItem *, PlotPolygon*> i(_plot_points);
        while (i.hasNext()) {
            i.next();
            PlotPolygon *pp = i.value();
            QPointF rt = Util::convertToXY(pp->_rt, zoom);
            QPointF rb = Util::convertToXY(pp->_rb, zoom);
            QPointF lt = Util::convertToXY(pp->_lt, zoom);
            QPointF lb = Util::convertToXY(pp->_lb, zoom);
            QGraphicsPolygonItem * signal_point = i.key();
            QPen pen = pp->_pen;
            QBrush brush = pp->_brush;
            _view->_childView->scene()->removeItem(signal_point);
            delete signal_point;
            _plot_points.remove(signal_point);
            //signal_point->setPos(xypos);
            QPolygonF poly;
            poly << lb << lt << rt << rb;
            QGraphicsPolygonItem *s_point = _view->_childView->scene()->addPolygon(poly,pen,brush);
            _plot_points.insert(s_point, pp);
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

    // restore aprs stations from previous sessions
    QVector<AprsStation *> aprs_stations = _db->select_aprs_stations();
    for (int i=0;i<aprs_stations.size();++i)
    {
        AprsStation *st = aprs_stations.at(i);
        QString filename = ":aprs/aprs_icons/slice_";
        QString icon = st->getImage();
        filename.append(icon).append(".png");
        QPixmap pixmap(filename);
        pixmap = pixmap.scaled(16,16);
        AprsPixmapItem *pic = new AprsPixmapItem(pixmap);
        pic->setAcceptHoverEvents(true);

        _view->_childView->scene()->addItem(pic);
        QPointF pos = QPointF(st->longitude,st->latitude);
        int zoom = _view->zoomLevel();
        QPointF xypos = Util::convertToXY(pos, zoom);
        pic->setMessage(st->callsign,st->via,st->message);
        pic->setPosition(xypos);
        pic->setOffset(xypos - QPoint(7,25));
        AprsIcon ic;
        ic.position = pos;
        ic.icon = icon;
        _map_aprs.insert(pic, ic);

        QString callsign_text;
        QRegularExpression re(";(.+?)\\*");
        QRegularExpressionMatch match = re.match(st->payload);
        if(match.hasMatch())
        {
            callsign_text = match.captured(1);
        }
        else
        {
            callsign_text = st->callsign;
        }
        QGraphicsTextItem * callsign = new QGraphicsTextItem;
        callsign->setPos(xypos - QPoint(0,16));
        callsign->setPlainText(callsign_text);

        _view->_childView->scene()->addItem(callsign);
        _map_aprs_text.insert(callsign,pos);
        delete st;
    }
    aprs_stations.clear();

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
            gs_form->ui->longitudeEdit->setText(QString::number(gs->longitude));
            gs_form->ui->latitudeEdit->setText(QString::number(gs->latitude));


            QDockWidget *dw = new QDockWidget;
            dw->setWindowTitle(QString::number( gs->id));
            dw->setMaximumWidth(260);
            dw->setWidget(gs_form);
            this->addDockWidget(Qt::LeftDockWidgetArea,dw);
            _docks.push_back(dw);
            //this->ui->dockWidget2->setWidget(gs_form);
            QObject::connect(gs_form,SIGNAL(haveData(GroundStation*)),this,SLOT(saveGroundStation(GroundStation *)));
            QObject::connect(gs_form,SIGNAL(delStation(unsigned)),this,SLOT(deleteGroundStation(unsigned)));
            QObject::connect(gs_form,SIGNAL(plotStation(GroundStation*)),this,SLOT(plotCoverage(GroundStation*)));
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


void MainWindow::startStandalone()
{
    _tb->ui->startStandaloneButton->setEnabled(false);
    _tb->ui->startStandaloneButton->setStyleSheet("background:rgb(220,220,220);");
    _tb->ui->stopStandaloneButton->setEnabled(true);
    _tb->ui->stopStandaloneButton->setStyleSheet("background:yellow;");

    _start_time= QDateTime::currentDateTime().toString("d/MMM/yyyy hh:mm:ss");
    QThread *t= new QThread;
    FGRadio *radiosystem = new FGRadio(_db);
    radiosystem->moveToThread(t);
    connect(radiosystem, SIGNAL(haveMobilePosition(double,double)), this, SLOT(moveMobile(double,double)));
    connect(radiosystem, SIGNAL(haveSignalReading(double, double, uint,QString,double,Signal*)), this, SLOT(showSignalReading(double, double, uint,QString,double,Signal*)));
    QObject::connect(_tb->ui->nextWaypointButton,SIGNAL(clicked()),this,SLOT(sequenceWaypoint()));
    connect(t, SIGNAL(started()), radiosystem, SLOT(update()));
    connect(radiosystem, SIGNAL(finished()), t, SLOT(quit()));
    connect(radiosystem, SIGNAL(finished()), radiosystem, SLOT(deleteLater()));
    connect(t, SIGNAL(finished()), t, SLOT(deleteLater()));
    _radio_subsystem = radiosystem;
    t->start();


}


void MainWindow::stopStandalone()
{
    QObject::disconnect(_tb->ui->nextWaypointButton,SIGNAL(clicked()),this,SLOT(sequenceWaypoint()));
    _radio_subsystem->stop();
    _tb->ui->startStandaloneButton->setEnabled(true);
    _tb->ui->startStandaloneButton->setStyleSheet("background:yellow;");
    _tb->ui->stopStandaloneButton->setEnabled(false);
    _tb->ui->stopStandaloneButton->setStyleSheet("background:rgb(220,220,220);");
}

void MainWindow::sequenceWaypoint()
{
    if(_radio_subsystem!=NULL)
    {
        _radio_subsystem->nextWaypoint();
    }
}


void MainWindow::sendFlightgearData()
{
    QObject::connect(_remote, SIGNAL(readyUpdate()), this, SLOT(startSignalUpdate()));
    _remote->sendAllData();
}

void MainWindow::startSignalUpdate()
{
    _tb->ui->startUpdateButton->setEnabled(false);
    _tb->ui->startUpdateButton->setStyleSheet("background:rgb(220,220,220);");
    _tb->ui->stopUpdateButton->setEnabled(true);
    _tb->ui->stopUpdateButton->setStyleSheet("background:yellow;");

    _start_time= QDateTime::currentDateTime().toString("d/MMM/yyyy hh:mm:ss");
    QThread *t= new QThread;
    Updater *up = new Updater(_db);
    up->moveToThread(t);
    connect(up, SIGNAL(haveMobilePosition(double,double)), this, SLOT(moveMobile(double,double)));
    connect(up, SIGNAL(haveSignalReading(double, double, uint,QString,double,Signal*)), this, SLOT(showSignalReading(double, double, uint,QString,double,Signal*)));

    connect(t, SIGNAL(started()), up, SLOT(startUpdate()));
    connect(up, SIGNAL(finished()), t, SLOT(quit()));
    connect(up, SIGNAL(finished()), up, SLOT(deleteLater()));
    connect(t, SIGNAL(finished()), t, SLOT(deleteLater()));
    QObject::connect(_telnet,SIGNAL(haveProperty(QString)),this,SLOT(setReceived(QString)),Qt::QueuedConnection);
    QObject::connect(up,SIGNAL(getProperty(QString)),_telnet,SLOT(getProperty(QString)),Qt::QueuedConnection);
    t->start();
    _updater = up;

}

void MainWindow::enableStartButton()
{
    _tb->ui->startUpdateButton->setEnabled(true);
    _tb->ui->startUpdateButton->setStyleSheet("background:yellow;");
    _tb->ui->stopUpdateButton->setEnabled(false);
    _tb->ui->stopUpdateButton->setStyleSheet("background:rgb(220,220,220);");
    QObject::connect(_tb->ui->startUpdateButton, SIGNAL(clicked()), this, SLOT(startSignalUpdate()));
}

void MainWindow::stopSignalUpdate()
{
    _updater->stop();
    _tb->ui->startUpdateButton->setEnabled(true);
    _tb->ui->startUpdateButton->setStyleSheet("background:yellow;");
    _tb->ui->stopUpdateButton->setEnabled(false);
    _tb->ui->stopUpdateButton->setStyleSheet("background:rgb(220,220,220);");
}

void MainWindow::setReceived(QString data)
{

    _updater->setReceived(data);
}


void MainWindow::moveMobile(double lon, double lat)
{
    if(_map_mobiles.size() > 0)
    {
        QMap<QGraphicsPixmapItem *, QPointF>::const_iterator it = _map_mobiles.begin();
        QPointF pos = it.value();
        /** now why did I do this for?? I can't remember
        if( (fabs(pos.rx()-lon)> 0.1) || ( fabs(pos.ry()-lat)>0.1) )
        {
            return;
        }
        */
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
                    /** now why did I do this for??
                    if( (fabs(pos.rx()-lon)> 0.1) || ( fabs(pos.ry()-lat)>0.1) )
                    {
                        continue;
                    }
                    */
                }
                QPointF gs_pos(gs->longitude,gs->latitude);
                QPointF mobile_pos(lon,lat);
                QPointF xy_gs_pos = Util::convertToXY(gs_pos,_view->zoomLevel());
                QPointF xy_mobile_pos = Util::convertToXY(mobile_pos,_view->zoomLevel());
                QLineF signal_line(xy_gs_pos,xy_mobile_pos);
                int alpha = 254;
                QColor colour = Util::getScaleColor(s->signal, alpha);
                QBrush brush(colour);

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
                signal_form->ui->distance->setText(QString::number(s->distance));
                signal_form->ui->rxHeight->setText(QString::number(s->rx_height));
                signal_form->ui->txHeight->setText(QString::number(s->tx_height));
                signal_form->ui->txERP->setText(QString::number(s->tx_erp));
                signal_form->ui->signal->setText(QString::number(s->signal));
                signal_form->ui->signalDbm->setText(QString::number(s->signal_dbm));
                signal_form->ui->fieldStrength->setText(QString::number(s->field_strength_uv));
                signal_form->ui->terrainAttenuation->setText(QString::number((s->terrain_attenuation)));
                signal_form->ui->clutterAttenuation->setText(QString::number(s->clutter_attenuation));
                signal_form->ui->linkBudget->setText(QString::number(s->link_budget));
                signal_form->ui->propMode->setText(s->prop_mode);

                double img_width=s->signal / (3 *12);
                if(img_width < 0) img_width=0.01;
                if(img_width > 1) img_width=1;
                int width = (int) rint(img_width*191);
                signal_form->ui->labelSmeter->setFixedWidth(width);




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
                        QLabel * distance = w->findChild<QLabel *>("distance");
                        QLabel * rxHeight = w->findChild<QLabel *>("rxHeight");
                        QLabel * txHeight = w->findChild<QLabel *>("txHeight");
                        QLabel * txERP = w->findChild<QLabel *>("txERP");
                        QLabel * signal = w->findChild<QLabel *>("signal");
                        QLabel * signalDbm = w->findChild<QLabel *>("signalDbm");
                        QLabel * fieldStrength = w->findChild<QLabel *>("fieldStrength");
                        QLabel * terrainAttenuation = w->findChild<QLabel *>("terrainAttenuation");
                        QLabel * clutterAttenuation = w->findChild<QLabel *>("clutterAttenuation");
                        QLabel * linkBudget = w->findChild<QLabel *>("linkBudget");
                        QLabel * propMode = w->findChild<QLabel *>("propMode");
                        QLabel * labelSmeter = w->findChild<QLabel *>("labelSmeter");

                        stationName->setText(station_name);
                        frequency->setText(QString::number(freq));
                        distance->setText(QString::number(s->distance));
                        rxHeight->setText(QString::number(s->rx_height));
                        txHeight->setText(QString::number(s->tx_height));
                        txERP->setText(QString::number(s->tx_erp));
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
                        terrainAttenuation->setText(QString::number(s->terrain_attenuation));
                        clutterAttenuation->setText(QString::number(s->clutter_attenuation));
                        linkBudget->setText(QString::number(s->link_budget));
                        propMode->setText(s->prop_mode);
                        double img_width=s->signal / (3 *12);
                        if(img_width < 0) img_width=0.01;
                        if(img_width > 1) img_width=1;
                        int width = (int) rint(img_width*191);
                        labelSmeter->setFixedWidth(width);

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

void MainWindow::plotCoverage(GroundStation *g)
{
    QMapIterator<QGraphicsPolygonItem *, PlotPolygon*> i(_plot_points);
    while (i.hasNext()) {
        i.next();
        _view->_childView->scene()->removeItem(i.key());
        delete i.key();
        delete i.value();
    }
    _plot_points.clear();
    for(int k=0;k<_plotvalues->size();++k)
    {
        delete _plotvalues->at(k);
    }
    _plotvalues->clear();
    QThread *t= new QThread;
    FGRadio *radiosystem = new FGRadio(_db);
    QPointF plot_pos(g->longitude,g->latitude);
    QPointF xy_plot_pos = Util::convertToXY(plot_pos,_view->zoomLevel());
    //_painted_pix->setOffset(xy_plot_pos - QPointF(500,500));
    _painted_pix->setOpacity(0.2);

    radiosystem->setPlotStation(g);
    radiosystem->moveToThread(t);
    connect(radiosystem, SIGNAL(havePlotPoint(double,double,double, double, double, double)),
            this, SLOT(drawPlot(double,double,double, double, double, double)));
    connect(radiosystem, SIGNAL(nrOfPos(int)), this, SLOT(setPlotProgressBar(int)));
    connect(t, SIGNAL(started()), radiosystem, SLOT(plot()));
    connect(radiosystem, SIGNAL(finished()), t, SLOT(quit()));
    connect(radiosystem, SIGNAL(finished()), this, SLOT(plottingFinished()));
    connect(radiosystem, SIGNAL(finished()), radiosystem, SLOT(deleteLater()));
    connect(t, SIGNAL(finished()), t, SLOT(deleteLater()));
    _radio_subsystem = radiosystem;
    t->start();

}

void MainWindow::setPlotProgressBar(int ticks)
{
    _tb->ui->progressBar->setVisible(true);
    _tb->ui->progressBar->setValue(3);

    _plot_progress_bar = 97/(double)ticks;
    _plot_progress_bar_value = 3;
}

void MainWindow::drawPlot(double lon, double lat, double lon1, double lat1, double distance, double signal)
{
    if(signal >0)
    {

        QPointF plot_pos(lon,lat);
        QPointF plot_pos1(lon+0.001*sqrt(2)*distance/100*sin(SGD_PI/180),lat+0.001*sqrt(2)*distance/100*sin(SGD_PI/180));
        QPointF plot_pos2(lon+0.001*sqrt(2)*distance/100*sin(SGD_PI/180),lat);
        QPointF plot_pos3(lon,lat+0.001*sqrt(2)*distance/100*sin(SGD_PI/180));
        QPointF plot_pos_a(lon1,lat1);
        QPointF xy_plot_pos = Util::convertToXY(plot_pos,_view->zoomLevel());
        QPointF xy_plot_pos1 = Util::convertToXY(plot_pos1,_view->zoomLevel());
        QPointF xy_plot_pos2 = Util::convertToXY(plot_pos2,_view->zoomLevel());
        QPointF xy_plot_pos3 = Util::convertToXY(plot_pos3,_view->zoomLevel());
        QPointF xy_plot_pos_a = Util::convertToXY(plot_pos_a,_view->zoomLevel());
        if(_last_plot_point.rx()==0 || _last_plot_point.ry()==0)
        {
            _last_plot_point.setX(xy_plot_pos.rx()+0.001);
            _last_plot_point.setY(xy_plot_pos.ry()+0.001);
        }
        int alpha = _plot_opacity;
        QColor colour = Util::getScaleColor(signal, alpha);
        QBrush brush(colour);
        QPen pen(brush, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        QPolygonF poly;
        poly << xy_plot_pos  << xy_plot_pos3 << xy_plot_pos1 << xy_plot_pos2;
        //poly << xy_plot_pos  << xy_plot_pos_a << _last_plot_point;

        /*
        QPainter painter(_plot_pixmap);
        painter.setBrush(brush);
        painter.setPen(pen);
        painter.drawPolygon(poly);
        */


        QGraphicsPolygonItem *polygon = _view->_childView->scene()->addPolygon(poly,pen,brush);
        //polygon->setZValue(1000);
        PlotPolygon *pp = new PlotPolygon;
        pp->_lb = plot_pos;
        pp->_rt = plot_pos1;
        pp->_rb = plot_pos2;
        pp->_lt = plot_pos3;
        pp->_brush = brush;
        pp->_pen = pen;

        _plot_points.insert(polygon,pp);
        _last_plot_point = xy_plot_pos;
        _plot_progress_bar_value += _plot_progress_bar;

        _tb->ui->progressBar->setValue(_plot_progress_bar_value);
        PlotValue *values = new PlotValue;
        values->longitude = lon;
        values->latitude = lat;
        values->signal = signal;
        values->distance = distance;
        _plotvalues->append(values);

    }

}

void MainWindow::changePlotOpacity(int opacity)
{
    _plot_opacity = opacity;
}

void MainWindow::plottingFinished()
{
    //_painted_pix->setPixmap(*_plot_pixmap);
    _tb->ui->progressBar->setVisible(false);
}

void MainWindow::savePlot()
{
    if(_plotvalues->size() > 0)
    {
        ofstream file_save("plot.txt");
        for(int i=0;i<_plotvalues->size();++i)
        {
            PlotValue *value= _plotvalues->at(i);
            double lon = value->longitude;
            double lat = value->latitude;
            double signal = value->signal;
            double distance = value->distance;
            file_save << lon << " " << lat << " " << signal << " " << distance << "\n";
        }
    }
}

void MainWindow::loadPlot()
{
    ifstream file_load("plot.txt");
    double lon, lat, signal, distance;
    while(!file_load.eof())
    {
        file_load >> lon >> lat >> signal >> distance;
        drawPlot(lon,lat,0.0,0.0,distance,signal);
    }

}
