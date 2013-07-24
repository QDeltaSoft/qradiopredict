#include "fgtelnet.h"

static QString CRLF ="\r\n";

FGTelnet::FGTelnet()
{
    QObject::connect(&_socket,SIGNAL(error(QAbstractSocket::SocketError )),this,SLOT(connectionFailed(QAbstractSocket::SocketError)));
    QObject::connect(&_socket,SIGNAL(connected()),this,SLOT(connectionSuccess()));
    _connection_tries=0;
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
    }
}


void FGTelnet::connectToFGFS()
{
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


void FGTelnet::runCmd(QString cmd)
{
    QString command = "run " + cmd + CRLF;
    _socket.write(command.toUtf8());
    _socket.flush();

}
