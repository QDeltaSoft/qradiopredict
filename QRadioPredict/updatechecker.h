#ifndef UPDATECHECKER_H
#define UPDATECHECKER_H

#include <QObject>

class UpdateChecker : public QObject
{
    Q_OBJECT
public:
    explicit UpdateChecker(QObject *parent = 0);
    
signals:
    
public slots:
    
};

#endif // UPDATECHECKER_H
