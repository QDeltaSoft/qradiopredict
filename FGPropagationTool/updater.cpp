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
        QTime delaytime= QTime::currentTime().addSecs(3);
        while( QTime::currentTime() < delaytime )
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
        QString signal = _fg->getProperty("/sim/radio/station[1]/signal");

        qDebug() << "signal: " << signal;
    }

}
