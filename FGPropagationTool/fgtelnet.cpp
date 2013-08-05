#include "fgtelnet.h"

static QString CRLF ="\r\n";

FGTelnet::FGTelnet()
{
    QObject::connect(&_socket,SIGNAL(error(QAbstractSocket::SocketError )),this,SLOT(connectionFailed(QAbstractSocket::SocketError)));
    QObject::connect(&_socket,SIGNAL(connected()),this,SLOT(connectionSuccess()));
    _connection_tries=0;
    _status=0;
    this->connectToFGFS();

    //if (_socket.waitForConnected(2000))
    //    qDebug("Connected!");
    //else qDebug("Could not connect to Flightgear!");
}

FGTelnet::~FGTelnet()
{
    _socket.disconnectFromHost();
}


void FGTelnet::connectionSuccess()
{
    qDebug("Connection to Flightgear established.");
    this->runCmd(QString("data"));
    _status=1;
    emit connectedToFGFS();
}


void FGTelnet::connectionFailed(QAbstractSocket::SocketError error)
{
    qDebug("OOps! Could not connect to FGFS. Trying again.");
    _connection_tries++;
    if(_connection_tries < 10)
    {
        this->connectToFGFS();
    }
    else
    {
        qDebug("Giving up! Flightgear is not running.");
        emit connectionFailure();
    }
    _status=0;
}


void FGTelnet::connectToFGFS()
{
    if(_status==1) return;
    _socket.connectToHost("localhost", 5500);
    //if (_socket.waitForConnected(2000))
    //    qDebug("Connected!");
    //else qDebug("Could not connect to Flightgear!");
}


void FGTelnet::setProperty(QString prop_name, QString value)
{
    QString command = "set " + prop_name + " " + value + CRLF;
    _socket.write(command.toUtf8());
    _socket.flush();

}

QString FGTelnet::getProperty(QString prop_name)
{
    qDebug() << "prop: " << prop_name;
    QString command = "get " + prop_name + CRLF;
    _socket.write(command.toUtf8());
    _socket.flush();
    char value[4096];
    if(_socket.canReadLine())
    {
        qint64 linelength = _socket.readLine(value,sizeof(value));
        if(linelength != -1)
        {
            QString response = QString(value);
            qDebug() << "response: " << response;
            QStringList l = response.split("'");
            if(l.size() >=3)
            {
                return QString(l[1]);
            }
            else
            {
                return QString("1");
            }
        }
    }
    return QString("1");

}

void FGTelnet::cd(QString dir)
{
    qDebug() << "dir: " << dir;
    QString command = "cd " + dir + " " +CRLF;
    _socket.write(command.toUtf8());
    _socket.flush();
}


void FGTelnet::runCmd(QString cmd)
{
    QString command = "run " + cmd + CRLF;
    _socket.write(command.toUtf8());
    _socket.flush();

}
