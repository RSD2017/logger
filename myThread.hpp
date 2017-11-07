#pragma once

#include <QtCore>
#include <QDebug>
#include <chrono>
#include <future>
#include <iostream>
#include <QMutexLocker>
#include <unistd.h>

class myThread : public QObject
{
    Q_OBJECT

    public:
        myThread( QMutex* _mutex, std::string *_buffer );
        ~myThread();

    public slots:
    // protected slots:
        void process();

    private:
        QMutex* mutex;
        std::string *buffer;
};
