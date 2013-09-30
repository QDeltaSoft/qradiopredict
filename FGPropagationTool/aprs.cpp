#include "aprs.h"

Aprs::Aprs(QString aprs_server)
{
    _socket = new QTcpSocket;
    QObject::connect(_socket,SIGNAL(error(QAbstractSocket::SocketError )),this,SLOT(connectionFailed(QAbstractSocket::SocketError)));
    QObject::connect(_socket,SIGNAL(connected()),this,SLOT(connectionSuccess()));
    QObject::connect(_socket,SIGNAL(readyRead()),this,SLOT(processData()));
    _connection_tries=0;
    _status=0;
    _authenticated = 0;
    _hostname = aprs_server;
    _delaytime = QTime::currentTime();
    this->connectToAPRS();
}


Aprs::~Aprs()
{
    _socket->disconnectFromHost();
    delete _socket;
}


void Aprs::connectionSuccess()
{
    qDebug("Connection to APRS established.");

    _status=1;
    _connection_tries=0;

    this->authenticate();

    emit connectedToAPRS();
}


void Aprs::connectionFailed(QAbstractSocket::SocketError error)
{
    _status=0;
    qDebug("OOps! Could not connect to APRS. Trying again.");
    _connection_tries++;
    if(_connection_tries < 10)
    {
        this->connectToAPRS();
    }
    else
    {
        qDebug("Giving up! APRS is not reachable.");
        emit connectionFailure();
    }

}


void Aprs::connectToAPRS()
{
    if(_status==1) return;
    _socket->connectToHost(_hostname, 14580);

}

void Aprs::disconnectAPRS()
{
    if(_status==0) return;
    _socket->disconnectFromHost();

}

void Aprs::authenticate()
{
    _socket->write("user XASTIR pass -1 filter r/46.00/26.00/200\r\n");
    _socket->flush();
    qDebug() << "Sent APRS login";

}

void Aprs::processData()
{
    if (_status!=1) return;
    QString response;

    while(_socket->bytesAvailable()>0)
    {
        QByteArray a = _socket->read(1024);
        response.append(a);
    }
    qDebug() << response;

    if(response.contains("verified"))
    {
        _authenticated=1;
        return;
    }
    if(response.startsWith("#"))
    {
        //discard comment
        return;
    }
    else
    {
        //we have a station report
        emit rawAprsData(response);
        QStringList v = response.split(":");
        QStringList v1 = v[0].split(">");
        QString from = v1[0];
        QStringList v2 = v1[1].split(",");
        unsigned via_size = v2.size();
        QString to = v2[0];
        QString via;
        for(uint i = 1;i<via_size ; ++i)
        {
            via.append(v2[i]).append(",");
        }

        QString payload = v[1];


        QString lat;
        QString lon;
        QRegularExpression re("(\\d\\d\\d\\d\\.\\d\\d\\w)(.)*(\\d\\d\\d\\d\\d.\\d\\d\\w)(\\S)(.+)");
        QRegularExpressionMatch match = re.match(payload);
        if (match.hasMatch()) {
            lat = match.captured(1);
            lon = match.captured(3);
            QString symbol = match.captured(4);
            QString message = match.captured(5);
            QString lat_degrees;
            QString lon_degrees;
            lat_degrees.append(lat[0]).append(lat[1]);
            lon_degrees.append(lon[0]).append(lon[1]).append(lon[2]);
            lat.chop(1);
            lon.chop(1);
            lat.remove(0,2);
            lon.remove(0,3);
            double lat_minutes = lat.toDouble();
            double lon_minutes = lon.toDouble();
            double latitude = lat_degrees.toDouble();
            double longitude = lon_degrees.toDouble();
            latitude = latitude + lat_minutes / 60;
            longitude = longitude + lon_minutes / 60;
            AprsStation *st = new AprsStation;
            st->adressee=to;
            st->callsign = from;
            st->via = via;
            st->symbol = symbol;
            st->payload = payload;
            st->message = message;
            st->latitude = latitude;
            st->longitude = longitude;
            QDateTime dt = QDateTime::currentDateTime();
            st->time_seen = dt.toTime_t();
            emit aprsData(st);

        }



    }

}

void Aprs::setFilter(QPointF &pos, int &range)
{
    if ((_status!=1)) return;
    if( QTime::currentTime() < _delaytime ) return;

    QString query = "#filter r/"+QString::number(pos.ry())+"/"+QString::number(pos.rx())+"/0"+QString::number(range)+"\r\n";

    _socket->write(query.toLatin1());
    _socket->flush();
    _delaytime = QTime::currentTime().addSecs(1);

}
