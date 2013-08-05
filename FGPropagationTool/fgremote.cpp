#include "fgremote.h"


const double SG_METER_TO_FEET = 3.28083989501312335958;

FGRemote::FGRemote(FGTelnet *t, DatabaseApi * db)
{
    _fg = t;
    _db = db;
}

FGRemote::~FGRemote()
{
}


void FGRemote::sendAllData()
{
    this->set_mobile();
    QTime delaytime= QTime::currentTime().addSecs(10);
    while( QTime::currentTime() < delaytime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    this->set_ground();
    this->set_fp();
}

void FGRemote::set_mobile(unsigned id)
{
    if(_fg->status())
    {
        /*
        while(true)
        {          
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
            if(_fg->getProperty("/sim/time/elapsed-sec").toDouble() > 5.0)
                break;
        }
        */

        QVector<MobileStation*> stations = _db->select_mobile_station(id);
        MobileStation *s = stations[0];
        QString str1;

        _fg->setProperty("/sim/presets/longitude-deg",str1.setNum(s->longitude));
        _fg->setProperty("/sim/presets/latitude-deg",str1.setNum(s->latitude));
        _fg->setProperty("/sim/presets/offset-azimuth","");
        _fg->setProperty("/sim/presets/heading-deg",str1.setNum(s->heading_deg));

        if(s->elevation_feet != 0)
        {
            _fg->setProperty( "/sim/presets/altitude-ft", str1.setNum(s->elevation_feet));
        }
        else
        {

            _fg->setProperty("/sim/radio/mobile/nasal/script",
                             "var f= 0; var d = geodinfo("+
                             str1.setNum(s->latitude)+
                             ", "+str1.setNum(s->longitude)+
                             ", 20000); if(d != nil) setprop('/sim/presets/altitude-ft', (d[0]+3)*"+
                             str1.setNum(SG_METER_TO_FEET)+");else setprop('/sim/presets/altitude-ft', 9);");
            _fg->runCmd("nasal /sim/radio/mobile/nasal");
        }

        _fg->setProperty("/instrumentation/comm[0]/frequencies",str1.setNum(s->frequency));

        _fg->runCmd("presets-commit");

    }
}

void FGRemote::set_ground(unsigned id)
{
    if(_fg->status())
    {
        /*
        while(true)
        {
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
            if(_fg->getProperty("/sim/sceneryloaded") =="true")
                break;
        }
        */

        QVector<GroundStation*> stations = _db->select_ground_stations(id);
        for (int i = 0;i< stations.size();++i)
        {

            GroundStation *s = stations.at(i);
            QString str1;
            QString str2;
            QString str3;

            QString st ="["+str1.setNum(s->id)+"]";



            _fg->setProperty("/sim/radio/station"+st+"/name", "");
            _fg->setProperty("/sim/radio/station"+st+"/position/latitude-deg", str1.setNum(s->latitude));
            _fg->setProperty("/sim/radio/station"+st+"/position/longitude-deg", str2.setNum(s->longitude));


            _fg->setProperty("/sim/radio/station"+st+"/nasal/script", "var f= 0; var d = geodinfo("+str1.setNum(s->latitude)+", "+str2.setNum(s->longitude)
                             +", 20000);"
                             "if(d != nil) setprop('/sim/radio/station"+st+"/position/elevation-ft', (d[0]+1)*"+str3.setNum(SG_METER_TO_FEET)+");"
                             "else setprop('/sim/radio/station"+st+"/position/elevation-ft', 1);");
            _fg->runCmd("nasal /sim/radio/station"+st+"/nasal");


            _fg->setProperty("/sim/radio/station"+st+"/position/heading-deg" ,str1.setNum(s->heading_deg));
            _fg->setProperty("/sim/radio/station"+st+"/position/pitch-deg",str1.setNum(s->pitch_deg));
            _fg->setProperty("/sim/radio/station"+st+"/signal-type",str1.setNum(3));
            _fg->setProperty("/sim/radio/station"+st+"/loc-gs",str1.setNum(0));
            _fg->setProperty("/sim/radio/station"+st+"/beacon-delay",str1.setNum(s->beacon_delay));
            _fg->setProperty("/sim/radio/station"+st+"/polarization",str1.setNum(s->polarization));
            _fg->setProperty("/sim/radio/station"+st+"/tx-power-watt",str1.setNum(s->tx_power_watt));
            _fg->setProperty("/sim/radio/station"+st+"/rx-sensitivity",str1.setNum(s->rx_sensitivity));
            _fg->setProperty("/sim/radio/station"+st+"/frequency",str1.setNum(s->frequency));
            _fg->setProperty("/sim/radio/station"+st+"/rx-antenna-type",s->rx_antenna_type);
            _fg->setProperty("/sim/radio/station"+st+"/tx-antenna-type",s->tx_antenna_type);
            _fg->setProperty("/sim/radio/station"+st+"/tx-antenna-gain",str1.setNum(s->tx_antenna_gain));
            _fg->setProperty("/sim/radio/station"+st+"/rx-antenna-gain",str1.setNum(s->rx_antenna_gain));
            _fg->setProperty("/sim/radio/station"+st+"/tx-antenna-height",str1.setNum(s->tx_antenna_height));
            _fg->setProperty("/sim/radio/station"+st+"/rx-antenna-height",str1.setNum(s->rx_antenna_height));
            _fg->setProperty("/sim/radio/station"+st+"/tx-line-losses",str1.setNum(s->tx_line_losses));
            _fg->setProperty("/sim/radio/station"+st+"/rx-line-losses",str1.setNum(s->rx_line_losses));
            _fg->setProperty("/sim/radio/station"+st+"/enabled",str1.setNum(1));

        }

    }

}

void FGRemote::set_fp(unsigned id)
{
    if(_fg->status())
    {
        QString str;
        QString str2;
        QString str3;
        QString str4;
        QString str5;
        QString str6;
        QVector<MobileStation*> stations = _db->select_mobile_station(id);
        MobileStation *mobile = stations[0];

        // disable autopilot first
        _fg->setProperty("/autopilot/settings/gps-driving-true-heading","false");
        _fg->setProperty("/autopilot/locks/heading","");
        _fg->setProperty("/autopilot/settings/target-speed-kt", "0");
        _fg->setProperty("/autopilot/locks/speed", "");
        _fg->setProperty("/autopilot/locks/speed", "");
        _fg->setProperty("/autopilot/settings/true-heading-deg", "");

        // ok now proceed
        _fg->setProperty("/autopilot/route-manager/active", "false");
        _fg->setProperty("/autopilot/route-manager/input", "@clear");
        _fg->setProperty("/autopilot/route-manager/current-wp","-1");
        //_fg->setProperty("/autopilot/route-manager/input", "@posinit");

        /*
        while(true)
        {
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
            if(_fg->getProperty("/sim/sceneryloaded") =="true")
                break;
        }
        */

        QVector<FlightPlanPoints*> fp_points = _db->select_flightplan_positions(id);
        for (int i = 0;i< fp_points.size();++i)
        {


            FlightPlanPoints *fp = fp_points.at(i);
            if(fp->altitude !=0)
            {
                qDebug() << fp->altitude << " we should not be here";
                _fg->setProperty("/autopilot/route-manager/input", "" +
                                 str3.setNum(fp->longitude)+","+
                                 str2.setNum(fp->latitude)+"@"+str6.setNum(fp->altitude));
            }
            else
            {

                _fg->setProperty("/sim/radio/waypoint/nasal/script",
                                 "var f= 0; var d = geodinfo(" + str2.setNum(fp->latitude) + ", " + str3.setNum(fp->longitude) +
                                 ", 20000); if(d != nil) setprop('/sim/radio/waypoint/elevation-ft', (d[0]+2)*"
                                 + str4.setNum(SG_METER_TO_FEET) + ");else setprop('/sim/radio/waypoint/elevation-ft', 9);"
                                 "var alt=getprop('/sim/radio/waypoint/elevation-ft');"
                                 "setprop('/autopilot/route-manager/input',\""+str3.setNum(fp->longitude)+","+
                                 str2.setNum(fp->latitude)+ "@\"~alt);");

                _fg->runCmd("nasal /sim/radio/waypoint/nasal");

                /*
                // sleep 2 secs as nasal is processed
                QTime delaytime= QTime::currentTime().addSecs(3);
                while( QTime::currentTime() < delaytime )
                    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

                //_fg->cd("/sim/radio/waypoint");

                QString altitude = _fg->getProperty("/sim/radio/waypoint/elevation-ft");
                qDebug() << str3.setNum(fp->longitude) << " " << str2.setNum(fp->latitude) << " " << altitude << " waypoint coords";
                _fg->setProperty("/autopilot/route-manager/input", "" +
                                 str3.setNum(fp->longitude) + "," + str2.setNum(fp->latitude) + "@" + altitude);
                                 */
            }
        }

        _fg->setProperty("/autopilot/route-manager/input", "@activate");
        _fg->setProperty("/autopilot/route-manager/input", "@jump0");

        if(mobile->terrain_following == 0)
        {
            _fg->setProperty("/autopilot/locks/altitude","vnav");
        }
        else
        {
            _fg->setProperty("/autopilot/settings/target-agl-feet", "5");
            _fg->setProperty("/autopilot/settings/target-agl-ft","5");
            _fg->setProperty("/autopilot/locks/altitude","agl-hold");

        }

        _fg->setProperty("/autopilot/settings/gps-driving-true-heading","true");
        _fg->setProperty("/autopilot/locks/heading", "true-heading-hold");

        if(mobile->speed == 0)
        {
            _fg->setProperty("/autopilot/settings/target-speed-kt","30");
            _fg->setProperty("/engines/engine/speed-max-mps", "30");
        }
        else
        {
            _fg->setProperty("/autopilot/settings/target-speed-kt", str5.setNum(mobile->speed));
            _fg->setProperty("/engines/engine/speed-max-mps", str5.setNum(mobile->speed));
        }

        _fg->setProperty("/autopilot/locks/speed", "speed-with-throttle");
        _fg->setProperty("/autopilot/locks/speed", "THR");

    }

}
