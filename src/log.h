#ifndef LOG_H
#define LOG_H

#include <QDebug>

#define TIME_FORMAT "%c"

namespace Log {
    QDebug Info();
    QDebug Debug();
    QDebug Warning();
    QDebug Critical();
    void Fatal(std::string message);
    void Fatal(char *message);
};

#endif // LOG_H
