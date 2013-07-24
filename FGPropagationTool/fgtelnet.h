#ifndef FGTELNET_H
#define FGTELNET_H

#include <QTcpSocket>

class FGTelnet : public QObject
{
    Q_OBJECT
public:
    FGTelnet();
    ~FGTelnet();

    void setProperty(QString prop_name, QString value);
    void runCmd(QString cmd);

public slots:

    void connectToFGFS();

signals:

    void connectedToFGFS();

private:
    QTcpSocket _socket;
    unsigned _connection_tries;


private slots:
    void connectionSuccess();
    void connectionFailed(QAbstractSocket::SocketError error);

};

#endif // FGTELNET_H
