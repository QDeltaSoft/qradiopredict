#include "updater.h"

Updater::Updater( DatabaseApi * d)
{

    _db = d;
    _received= 0;

}

Updater::~Updater()
{

}

void Updater::setReceived(QString prop_data)
{

    _prop_data = prop_data;
    _received =1;
}

void Updater::startUpdate()
{

    QVector<GroundStation *> stations = _db->select_ground_stations(0);

    while(true)
    {

        //QTime delaytime= QTime::currentTime().addSecs(2);
        //while( QTime::currentTime() < delaytime ){}



        _received = 0;
        emit getProperty("/position/longitude-deg");
        while(_received==0)
        {

        }
        QString lon = _prop_data;

        _received = 0;
        double longitude = lon.toDouble();

        emit getProperty("/position/latitude-deg");
        while(_received==0){}
        QString lat = _prop_data;
        _received = 0;
        double latitude = lat.toDouble();

        emit haveMobilePosition(longitude,latitude);



        for(int i=0;i<stations.size();++i)
        {
            QString str1;
            GroundStation *g = stations.at(i);
            QString st ="["+str1.setNum(g->id)+"]";

            emit getProperty("/sim/radio/station"+st+"/signal");
            while(_received==0){}
            QString signal = _prop_data;
            _received = 0;

            emit getProperty("/sim/radio/station"+st+"/signal-dbm");
            while(_received==0){}
            QString signal_dbm = _prop_data;
            _received = 0;

            emit getProperty("/sim/radio/station"+st+"/field-strength-uV");
            while(_received==0){}
            QString field_strength_uv = _prop_data;
            _received = 0;

            emit getProperty("/sim/radio/station"+st+"/link-budget");
            while(_received==0){}
            QString link_budget = _prop_data;
            _received = 0;

            emit getProperty("/sim/radio/station"+st+"/terrain-attenuation");
            while(_received==0){}
            QString terrain_attenuation = _prop_data;
            _received = 0;

            emit getProperty("/sim/radio/station"+st+"/clutter-attenuation");
            while(_received==0){}
            QString clutter_attenuation = _prop_data;
            _received = 0;

            emit getProperty("/sim/radio/station"+st+"/prop-mode");
            while(_received==0){}
            QString prop_mode = _prop_data;
            _received = 0;

            Signal *s = new Signal;
            s->signal = signal.toDouble();
            s->signal_dbm = signal_dbm.toDouble();

            s->field_strength_uv = field_strength_uv.toDouble();
            s->link_budget = link_budget.toDouble();
            s->terrain_attenuation = terrain_attenuation.toDouble();
            s->clutter_attenuation = clutter_attenuation.toDouble();
            s->prop_mode = prop_mode;

            emit haveSignalReading(longitude,latitude,g->id,g->name,g->frequency,s);
            //QTime delaytime= QTime::currentTime().addSecs(1);
            //while( QTime::currentTime() < delaytime ) {}
        }


    }
    stations.clear();
    emit finished();

}
