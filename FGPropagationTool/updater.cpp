#include "updater.h"

Updater::Updater( DatabaseApi * d)
{
    _fg = new FGTelnet;
    _db = d;
}

Updater::~Updater()
{
    delete _fg;
}

void Updater::startUpdate()
{

    QVector<GroundStation *> stations = _db->select_ground_stations(0);

    while(true)
    {

        QTime delaytime= QTime::currentTime().addSecs(2);
        while( QTime::currentTime() < delaytime ) {}




        QString lon = _fg->getProperty("/position/longitude-deg");
        double longitude = lon.toDouble();
        QString lat = _fg->getProperty("/position/latitude-deg");
        double latitude = lat.toDouble();

        emit haveMobilePosition(longitude,latitude);



        for(int i=0;i<stations.size();++i)
        {
            QString str1;
            GroundStation *g = stations.at(i);
            QString st ="["+str1.setNum(g->id)+"]";
            QString signal = _fg->getProperty("/sim/radio/station"+st+"/signal");
            QString signal_dbm = _fg->getProperty("/sim/radio/station"+st+"/signal-dbm");
            QString field_strength_uv = _fg->getProperty("/sim/radio/station"+st+"/field-strength-uV");
            QString link_budget = _fg->getProperty("/sim/radio/station"+st+"/link-budget");
            QString terrain_attenuation = _fg->getProperty("/sim/radio/station"+st+"/terrain-attenuation");
            QString clutter_attenuation = _fg->getProperty("/sim/radio/station"+st+"/clutter-attenuation");
            QString prop_mode = _fg->getProperty("/sim/radio/station"+st+"/prop-mode");
            Signal *s = new Signal;
            s->signal = signal.toDouble();
            s->signal_dbm = signal_dbm.toDouble();
            s->field_strength_uv = field_strength_uv.toDouble();
            s->link_budget = link_budget.toDouble();
            s->terrain_attenuation = terrain_attenuation.toDouble();
            s->clutter_attenuation = clutter_attenuation.toDouble();
            s->prop_mode = prop_mode;
            //_db->update_signals(g->id,0,s);
            //_db->update_replays(g->id,0,lon.toDouble(,lat.toDouble(),start_time,s);
            emit haveSignalReading(longitude,latitude,g->id,g->name,g->frequency,s);
        }


    }
    stations.clear();
    emit finished();

}
