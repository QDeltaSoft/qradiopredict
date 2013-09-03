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

class Aprs : public QObject
{
    Q_OBJECT
public:
    explicit Aprs(QObject *parent = 0);
    ~Aprs();
    void connectToAPRS();
    void queryall(QPointF &pos);
    
signals:
    void connectedToAPRS();
    void connectionFailure();
    void aprsData(AprsStation st);
    
public slots:
    void connectionSuccess();
    void connectionFailed(QAbstractSocket::SocketError error);
    void processData();

private:
    void authenticate();

    QTcpSocket *_socket;
    quint8 _connection_tries;
    unsigned _status;
    unsigned _authenticated;
    
};

#endif // APRS_H
