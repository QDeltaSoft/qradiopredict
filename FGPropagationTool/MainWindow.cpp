#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "toolbox.h"
#include "ui_toolbox.h"
#include "connectionsuccessdialog.h"
#include "ui_connectionsuccessdialog.h"

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

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    _telnet = new FGTelnet;
    _db = new DatabaseApi;

    ui->setupUi(this);
    //!!!!!!!! connections must always come after setupUi!!!
    QObject::connect(ui->actionConnect_to_Flightgear,SIGNAL(triggered()),this->_telnet,SLOT(connectToFGFS()));
    QObject::connect(this->_telnet,SIGNAL(connectedToFGFS()),this,SLOT(connectionSuccess()));

    //Setup the MapGraphics scene and view
    MapGraphicsScene * scene = new MapGraphicsScene(this);
    MapGraphicsView * view = new MapGraphicsView(scene,this);

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

    toolbox *tb = new toolbox();

    this->ui->dockWidget3->setWidget(tb);
    this->ui->centralWidget->setVisible(false);

    this->ui->menuWindow->addAction(this->ui->dockWidget->toggleViewAction());
    this->ui->menuWindow->addAction(this->ui->dockWidget3->toggleViewAction());
    this->ui->dockWidget->toggleViewAction()->setText("&Layers");
    this->ui->dockWidget3->toggleViewAction()->setText("&Toolbox");

    view->setZoomLevel(4);
    view->centerOn(-111.658752, 40.255456);
    view->_childView->viewport()->setCursor(Qt::ArrowCursor);
    WeatherManager * weatherMan = new WeatherManager(scene, this);
    Q_UNUSED(weatherMan)
}

MainWindow::~MainWindow()
{
    delete ui;
    delete _telnet;
    delete _db;
}

//private slot
void MainWindow::on_actionExit_triggered()
{
    this->close();
}

void MainWindow::connectionSuccess()
{
    ConnectionSuccessDialog *dialog = new ConnectionSuccessDialog;

    dialog->show();
    this->ui->dockWidget3->toggleViewAction()->setText("&Toolbox (active)");
}

