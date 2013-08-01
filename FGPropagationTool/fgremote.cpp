#include "fgremote.h"


const double SG_METER_TO_FEET = 3.28083989501312335958;

FGRemote::FGRemote(FGTelnet *t)
{
    _fg = t;
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
        QString str2;
        QString str3;

        _fg->setProperty("/sim/radio/station"+st+"/name", "");
        _fg->setProperty("/sim/radio/station"+st+"/position/latitude-deg", str1.setNum(pos.ry()));
        _fg->setProperty("/sim/radio/station"+st+"/position/longitude-deg", str2.setNum(pos.rx()));


        _fg->setProperty("/sim/radio/station"+st+"/nasal/script", "var f= 0; var d = geodinfo("+str1.setNum(pos.ry())+", "+str2.setNum(pos.rx())
                         +", 20000);"
                         "if(d != nil) setprop('/sim/radio/station"+st+"/position/elevation-ft', (d[0]+1)*"+str3.setNum(SG_METER_TO_FEET)+");"
                         "else setprop('/sim/radio/station"+st+"/position/elevation-ft', 1);");
        _fg->runCmd("nasal /sim/radio/station"+st+"/nasal");


    }

}

void FGRemote::set_fp(QPointF pos)
{

}
