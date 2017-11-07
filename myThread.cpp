#include "myThread.hpp"

myThread::myThread( QMutex* _mutex, std::string *_buffer )
{
    this->mutex = _mutex;
    this->buffer = _buffer;
}

myThread::~myThread() {
}

std::string FISKGetLineFromCin() {
    std::string line;
    std::getline(std::cin, line);
    return line;
}

void myThread::process()
{
    auto future = std::async(std::launch::async, FISKGetLineFromCin);
    auto line = future.get();
    if (!line.empty()) {
        std::cout << "Message: " << line << '\n';
        mutex->lock(); // To avoid collision
        usleep(10);
        (*buffer) = line;
        mutex->unlock(); // Let's release the lock
        usleep(1);
    }
}
