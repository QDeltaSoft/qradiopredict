#include "util.h"


const qreal PI = 3.14159265358979323846;

Util::Util()
{
}

Util::~Util()
{
}

QPointF Util::convertToLL(QPointF pos, double zoom)
{

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


void Util::startFlightgear(DatabaseApi *db)
{
    QProcess p;
    QStringList args;


    QVector<FlightgearPrefs *> prefs = db->select_prefs();
    QVector<MobileStation *> mobiles = db->select_mobile_station(0);
    double lon = -1;
    double lat = -1;
    if(mobiles.size()>0)
    {
        MobileStation *mobile = mobiles[0];
        lon = mobile->longitude;
        lat = mobile->latitude;
    }
    FlightgearPrefs *pref;
    if(prefs.size()>0)
    {
        pref = prefs[0];
        args << "--fg-root="+pref->_fgdata_path
                <<   "--aircraft="+pref->_aircraft;

    }
    else
    {

        args << "--fg-root=/home/adrian/games/fgfs/install/fgfs/fgdata/"
             <<   "--aircraft=ufo";
    }
    if((lon==-1) || (lat==-1))
    {
        if(prefs.size()>0)
        {
            args <<  "--airport=" + pref->_airport;
        }
        else
        {
            args  <<  "--airport=LRBC";
        }
    }
    if((lon!=-1) && (lat!=-1))
    {
        args <<   "--lon="+QString::number(lon)
            <<     "--lat="+QString::number(lat);
    }
    args
         <<       "--on-ground"
         <<       "--timeofday=noon"
         <<   "--control=joystick"
         <<   "--prop:/environment/metar/data='NIL\n'"
         <<   "--prop:input/mice/mouse/mode/button[2]/binding/value=2"
         <<   "--disable-real-weather-fetch"
         <<     "--fov=90"
         <<   "--visibility-miles=40"
         <<     "--disable-fullscreen";
    if(prefs.size()==-1 && pref->_windowX!=0 && pref->_windowY!=0)
    {
        args << "--geometry=" << QString::number(pref->_windowX) << "x" << QString::number(pref->_windowY);
    }
    else
    {
         args <<   "--geometry=1024x768";
    }
         args <<     "--disable-hud"
         <<     "--disable-hud-3d"
         <<   "--log-level=normal"
#if 1
           <<     "--prop:/sim/rendering/materials-file=Materials/my_mappings/materials.xml"
#endif
         <<     "--prop:/sim/radio/use-radio=true";
    if(prefs.size()>0)
    {
        args << "--fg-scenery=" + pref->_scenery_path;
    }
    else
    {
        args
         <<   "--fg-scenery=/home/adrian/games/fgfs/scenery:/home/adrian/games/fgfs/terrasync:/home/adrian/games/fgfs/install/fgfs/fgdata/Scenery";
    }
    args
         <<   "--prop:/sim/radio/use-clutter-attenuation=true"
         <<   "--prop:/sim/radio/use-antenna-pattern=true"
#if 1
         <<     "--telnet=socket,bi,100,,5500,tcp";
#else
         <<     "--telnet=5500";
#endif

    if(prefs.size()>0)
    {
        p.startDetached("/home/adrian/games/fgfs/install/fgfs/bin/fgfs", args);
    }
    else
    {
        p.startDetached(pref->_fgfs_bin, args);
    }
}


QVector<SGGeod*> Util::drawDisk(const SGGeod &center, const double &radius, const int &step_deg, const double &step_point)
{
    QVector<SGGeod*> disk;
    SGGeod *g = new SGGeod();
    g->setLatitudeDeg(center.getLatitudeDeg());
    g->setLongitudeDeg(center.getLongitudeDeg());
    g->setElevationM(g->getElevationM());
    disk.append(g);
    SGGeoc center_c = SGGeoc::fromGeod(center);
    for(int i=0;i<360;i+=step_deg)
    {
        double probe_distance = 0;
        for(int j=0;j<5000;++j) // supposed to limit distance
        {
            probe_distance += step_point;


            SGGeod probe = SGGeod::fromGeoc(center_c.advanceRadM( i*PI/180, probe_distance));

            double dist_to_center = SGGeodesy::distanceM(center, probe);
            if(dist_to_center >= radius)
            {
                break;
            }
            else
            {
                SGGeod *geod = new SGGeod();
                geod->setLatitudeDeg(probe.getLatitudeDeg());
                geod->setLongitudeDeg(probe.getLongitudeDeg());
                geod->setElevationM(probe.getElevationM());
                disk.append(geod);
            }
        }
    }
    return disk;
}



