#include "myThread.hpp"

myThread::myThread( QMutex* _mutex, std::vector<std::string> *_buffer )
{
    this->mutex = _mutex;
    this->buffer = _buffer;
}

myThread::~myThread() {
}

std::string GetLineFromCin() {
    std::string line;
    std::getline(std::cin, line);
    return line;
}

void myThread::process()
{
    auto future = std::async(std::launch::async, GetLineFromCin);
    std::string line = future.get();
    if (!line.empty()) {
        mutex->lock(); // To avoid collision
        usleep(10);
        (*buffer).push_back( line );
        mutex->unlock(); // Let's release the lock
        usleep(1);
        emit newOrder();
    }
}
