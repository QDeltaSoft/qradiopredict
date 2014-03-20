#include "updatechecker.h"

UpdateChecker::UpdateChecker(QObject *parent) :
    QObject(parent)
{
    _socket = new QTcpSocket;
    QObject::connect(_socket,SIGNAL(error(QAbstractSocket::SocketError)),
                     this,SLOT(connectionError()));
    QObject::connect(_socket,SIGNAL(connected()),this,
                     SLOT(readyWrite()));
    QObject::connect(_socket,SIGNAL(readyRead()),this,SLOT(processData()));

}


UpdateChecker::~UpdateChecker()
{
    delete _socket;
}

void UpdateChecker::connectionError()
{
    qDebug() << "Error checking for updates.";
    emit updateCheckerError();
}

void UpdateChecker::connectToServer()
{
    qDebug() << "connecting...";
    _socket->connectToHost("qradiopredict.sourceforge.net", 80);
}

void UpdateChecker::readyWrite()
{
    qDebug() << "connected to host";
    _socket->write("GET /version HTTP/1.0\n\n");
    _socket->flush();
}

void UpdateChecker::processData()
{
    qDebug() << "processing message";
    QByteArray buf;
    buf.append(_socket->readAll());

    bool endOfLine = false;

    while ((!endOfLine))
    {

        char ch;
        if(_socket->bytesAvailable()>0)
        {
            int bytesRead = _socket->read(&ch, sizeof(ch));
            if (bytesRead == sizeof(ch))
            {
                //cnt++;
                buf.append( ch );
                if(_socket->bytesAvailable()==0)
                {
                    endOfLine = true;

                }

            }
        }
        else
        {
            break;
        }


    }
    QString message(buf);
    QStringList parts = message.split("###",QString::SkipEmptyParts);
    if(parts.length() < 2)
    {
        qDebug() << "No version file found";
        emit noUpdateAvailable();
    }
    QString version= parts[1];
    qDebug() << version;

}
