#ifndef LOADEDFILE_H
#define LOADEDFILE_H

#include <QString>

#ifdef _WIN32
    #define SLASH "\\"
#else
    #define SLASH "/"
#endif

class LoadedFile
{
public:
    QString contents = "";
    QString fileName = "";
    std::string fullPath = "";
    bool savedSinceLastEdit = false;
};

#endif // LOADEDFILE_H
