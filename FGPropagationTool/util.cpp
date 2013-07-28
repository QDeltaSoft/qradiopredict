#include "util.h"
#include <QProcess>
#include <QStringList>
#include <cmath>

const qreal PI = 3.14159265358979323846;

Util::Util()
{
}

Util::~Util()
{
}

QPointF Util::convertToLL(QPointF pos, double zoom)
{
    //QRectF sr = _view->_childView->sceneRect();
    //qDebug() << sr.width() << " " << sr.height() << " " << _view->zoomLevel();



    const qreal tilesOnOneEdge = pow(2.0,zoom);
    const quint16 tileSize = 256;
    qreal longitude = (pos.rx()*(360/(tilesOnOneEdge*tileSize)))-180;
    qreal latitude = (180/PI)*(atan(sinh((1-pos.ry()*(2/(tilesOnOneEdge*tileSize)))*PI)));
    return QPointF(longitude,latitude);
}


QPointF Util::convertToXY(QPointF ll, double zoom)
{
    const qreal tilesOnOneEdge = pow(2.0,zoom);
    const quint16 tileSize = 256;
    qreal x = (ll.x()+180) * (tilesOnOneEdge*tileSize)/360; // coord to pixel!
    qreal y = (1-(log(tan(PI/4+(ll.y()*PI/180)/2)) /PI)) /2  * (tilesOnOneEdge*tileSize);

    return QPoint(int(x), int(y));
}


void Util::startFlightgear()
{
    QProcess p;
    QStringList args;
    args << "--fg-root=/home/adrian/games/fgfs/install/fgfs/fgdata/"
         <<   "--aircraft=ufo"
         <<     "--airport=LRBC"
         //<<   "--lon="
         //<<     "--lat="
         <<       "--on-ground"
         <<       "--timeofday=noon"
         <<   "--control=joystick"
         <<   "--prop:/environment/metar/data='NIL\n'"
         <<   "--prop:input/mice/mouse/mode/button[2]/binding/value=2"
         <<   "--disable-real-weather-fetch"
         <<     "--fov=90"
         <<   "--visibility-miles=40"
         <<     "--disable-fullscreen"
         <<   "--geometry=1024x768"
         <<     "--disable-hud"
         <<     "--disable-hud-3d"
         <<   "--log-level=normal"
         <<     "--prop:/sim/radio/use-radio=true"
         <<   "--fg-scenery=/home/adrian/games/fgfs/scenery:/home/adrian/games/fgfs/terrasync:/home/adrian/games/fgfs/install/fgfs/fgdata/Scenery"
         <<   "--prop:/sim/radio/use-clutter-attenuation=true"
         <<   "--prop:/sim/radio/use-antenna-pattern=true"
         <<     "--telnet=5500";
    p.startDetached("/home/adrian/games/fgfs/install/fgfs/bin/fgfs", args);
}



