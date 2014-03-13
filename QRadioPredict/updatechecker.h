#ifndef UPDATECHECKER_H
#define UPDATECHECKER_H

#include <QObject>
#include <QTcpSocket>

class UpdateChecker : public QObject
{
    Q_OBJECT
public:
    explicit UpdateChecker(QObject *parent = 0);
    ~UpdateChecker();
signals:
    
public slots:

private slots:
    void connectionError();
    void readyWrite();

private:
    QTcpSocket *_socket;
    
};

#endif // UPDATECHECKER_H
