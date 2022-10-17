#include "log.h"

#include <QThread>
#include <chrono>
#include <ctime>

QDebug Log::Info(){
    time_t t = time(NULL);
    struct tm * p = localtime(&t);
    char curTimeStr[100];
    strftime(curTimeStr, 100, TIME_FORMAT, p);

    QDebug logger = qInfo().nospace();
    logger << "[" << curTimeStr << "] [Thread " << QThread::currentThread() << "/INFO] ";
    return logger;
}

QDebug Log::Debug(){
    time_t t = time(NULL);
    struct tm * p = localtime(&t);
    char curTimeStr[100];
    strftime(curTimeStr, 100, TIME_FORMAT, p);

    QDebug logger = qDebug().nospace();
    logger << "[" << curTimeStr << "] [Thread " << QThread::currentThread() << "/DEBUG] ";
    return logger;
}

QDebug Log::Warning(){
    time_t t = time(NULL);
    struct tm * p = localtime(&t);
    char curTimeStr[100];
    strftime(curTimeStr, 100, TIME_FORMAT, p);

    QDebug logger = qWarning().nospace();
    logger << "[" << curTimeStr << "] [Thread " << QThread::currentThread() << "/WARNING] ";
    return logger;
}

QDebug Log::Critical(){
    time_t t = time(NULL);
    struct tm * p = localtime(&t);
    char curTimeStr[100];
    strftime(curTimeStr, 100, TIME_FORMAT, p);

    QDebug logger = qCritical().nospace();
    logger << "[" << curTimeStr << "] [Thread " << QThread::currentThread() << "/CRITICAL] ";
    return logger;
}

void Log::Fatal(std::string message){
    time_t t = time(NULL);
    struct tm * p = localtime(&t);
    char curTimeStr[100];
    strftime(curTimeStr, 100, TIME_FORMAT, p);

    qFatal("[%s] [Thread %s/FATAL] %s", curTimeStr, (char*) QThread::currentThreadId(), message.data());
}

void Log::Fatal(char *message){
    time_t t = time(NULL);
    struct tm * p = localtime(&t);
    char curTimeStr[100];
    strftime(curTimeStr, 100, TIME_FORMAT, p);

    qFatal("[%s] [Thread %s/FATAL] %s", curTimeStr, (char*) QThread::currentThreadId(), message);
}
