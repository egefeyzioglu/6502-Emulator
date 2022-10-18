#ifndef LOG_H
#define LOG_H

#include <QDebug>

#define TIME_FORMAT "%c"

/**
 * @brief Mirrors the functionality of the QDebug class, with more debug information
 *
 * Prints out the current system time, the current thread, and the type of message
 * before printing the message passed to it. Contrary to QDebug's behavior, does not
 * insert spaces between items sent to it.
 */
namespace Log {
    QDebug Info();
    QDebug Debug();
    QDebug Warning();
    QDebug Critical();
    void Fatal(std::string message);
    void Fatal(char *message);
};

#endif // LOG_H
