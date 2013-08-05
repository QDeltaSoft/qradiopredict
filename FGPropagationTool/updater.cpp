#include "updater.h"

Updater::Updater(FGTelnet *t, DatabaseApi * db)
{
    _fg = t;
    _db = db;
}

void Updater::startUpdate()
{
    while(true)
    {
        QTime delaytime= QTime::currentTime().addSecs(2);
        while( QTime::currentTime() < delaytime )
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);


        QString lon = _fg->getProperty("/position/longitude-deg");
        QString lat = _fg->getProperty("/position/latitude-deg");

        emit haveMobilePosition(lon.toDouble(),lat.toDouble());

        QString signal = _fg->getProperty("/sim/radio/station[1]/signal");

    }

}
