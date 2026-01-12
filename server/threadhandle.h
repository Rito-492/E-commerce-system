#ifndef THREADHANDLE_H
#define THREADHANDLE_H

#include <QDebug>
#include <QHash>
#include <QMap>
#include <QQueue>
#include <QThread>

class ThreadHandle
{
public:

    ~ThreadHandle();

    static ThreadHandle &getClass();
    QThread *getThread();
    QThread *findThread();
    void removeThread(QThread *th);
    void clear();

    bool initFlag;

private:

    ThreadHandle();
    const static int MAXSIZE = 5;
    QHash<QThread*, int>ThreadPool;
};

#endif // THREADHANDLE_H
