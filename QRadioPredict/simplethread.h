#ifndef SIMPLETHREAD_H
#define SIMPLETHREAD_H

#include <QThread>

class SimpleThread : public QThread
{
public:
    SimpleThread();
    void run();

};

#endif // SIMPLETHREAD_H
