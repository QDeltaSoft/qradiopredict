#ifndef APRS_H
#define APRS_H

#include <QObject>
#include <QTcpSocket>
#include <QPointF>
#include <QString>
#include <QStringList>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include "aprsstation.h"
#include <QTime>

class Aprs : public QObject
{
    Q_OBJECT
public:
    explicit Aprs( QString aprs_server);
    ~Aprs();
    void connectToAPRS();
    void disconnectAPRS();
    void setFilter(QPointF &pos);
    
signals:
    void connectedToAPRS();
    void connectionFailure();
    void aprsData(AprsStation *st);
    
public slots:
    void connectionSuccess();
    void connectionFailed(QAbstractSocket::SocketError error);
    void processData();

private:
    void authenticate();
    QString _hostname;
    QTcpSocket *_socket;
    quint8 _connection_tries;
    unsigned _status;
    unsigned _authenticated;
    QTime _delaytime;
    
};

#endif // APRS_H
