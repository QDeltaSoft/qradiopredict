#include "fgremote.h"


const double SG_METER_TO_FEET = 3.28083989501312335958;

FGRemote::FGRemote(FGTelnet *t, DatabaseApi * db)
{
    _fg = t;
    _db = db;
}

void FGRemote::set_mobile(QPointF pos)
{
    if(_fg->status())
    {
        /*
        while(true)
        {
            if(_fg->getProperty("/sim/time/elapsed-sec").toDouble() > 5.0)
                break;
        }
        */

        QString str1;
        QString str2;
        QString str3;
        _fg->setProperty("/sim/presets/longitude-deg",str1.setNum(pos.rx()));
        _fg->setProperty("/sim/presets/latitude-deg",str2.setNum(pos.ry()));
        _fg->setProperty("/sim/presets/offset-azimuth","");
        _fg->setProperty("/sim/presets/heading-deg",0);

        _fg->setProperty("/sim/radio/mobile/nasal/script",
                         "var f= 0; var d = geodinfo("+
                         str2.setNum(pos.ry())+
                         ", "+str1.setNum(pos.rx())+
                         ", 20000); if(d != nil) setprop('/sim/presets/altitude-ft', (d[0]+3)*"+
                         str3.setNum(SG_METER_TO_FEET)+");else setprop('/sim/presets/altitude-ft', 9);");
        _fg->runCmd("nasal /sim/radio/mobile/nasal");

        _fg->setProperty("/instrumentation/comm[0]/frequencies","118.5");

        _fg->runCmd("presets-commit");

        /*

        while(true)
        {
            if(_fg->getProperty("/sim/sceneryloaded")=="true")
                break;
        }
        */
    }
}

void FGRemote::set_ground(QPointF pos)
{
    if(_fg->status())
    {
        QString st ="[1]";
        QString str1;


        _fg->setProperty("/sim/radio/station"+st+"/name", "");
        _fg->setProperty("/sim/radio/station"+st+"/position/latitude-deg", str1.setNum(pos.ry()));
        _fg->setProperty("/sim/radio/station"+st+"/position/longitude-deg", str1.setNum(pos.rx()));


        _fg->setProperty("/sim/radio/station"+st+"/nasal/script", "var f= 0; var d = geodinfo("+str1.setNum(pos.ry())+", "+str1.setNum(pos.rx())
                         +", 20000);"
                         "if(d != nil) setprop('/sim/radio/station"+st+"/position/elevation-ft', (d[0]+1)*"+str1.setNum(SG_METER_TO_FEET)+");"
                         "else setprop('/sim/radio/station"+st+"/position/elevation-ft', 1);");
        _fg->runCmd("nasal /sim/radio/station"+st+"/nasal");

/* TODO:
        _fg->setProperty("/sim/radio/station"+st+"/position/heading-deg" ,heading_deg);
        _fg->setProperty("/sim/radio/station"+st+"/position/pitch-deg",pitch_deg);
        _fg->setProperty("/sim/radio/station"+st+"/signal-type",3);
        _fg->setProperty("/sim/radio/station"+st+"/loc-gs",0);
        _fg->setProperty("/sim/radio/station"+st+"/beacon-delay",beacon_delay);
        _fg->setProperty("/sim/radio/station"+st+"/polarization",polarization);
        _fg->setProperty("/sim/radio/station"+st+"/tx-power-watt",tx_power_watt);
        _fg->setProperty("/sim/radio/station"+st+"/rx-sensitivity",rx_sensitivity);
        _fg->setProperty("/sim/radio/station"+st+"/frequency",frequency);
        _fg->setProperty("/sim/radio/station"+st+"/rx-antenna-type",rx_antenna_type);
        _fg->setProperty("/sim/radio/station"+st+"/tx-antenna-type",tx_antenna_type);
        _fg->setProperty("/sim/radio/station"+st+"/tx-antenna-gain",tx_antenna_gain);
        _fg->setProperty("/sim/radio/station"+st+"/rx-antenna-gain",rx_antenna_gain);
        _fg->setProperty("/sim/radio/station"+st+"/tx-antenna-height",tx_antenna_height);
        _fg->setProperty("/sim/radio/station"+st+"/rx-antenna-height",rx_antenna_height);
        _fg->setProperty("/sim/radio/station"+st+"/tx-line-losses",tx_line_losses);
        _fg->setProperty("/sim/radio/station"+st+"/rx-line-losses",rx_line_losses);
        _fg->setProperty("/sim/radio/station"+st+"/enabled",1);
*/

    }

}

void FGRemote::set_fp(QPointF pos)
{

}
