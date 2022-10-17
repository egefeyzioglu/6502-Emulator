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
    QString file_name = "";
    std::string full_path = "";
    bool saved_since_last_edit = false;
};

#endif // LOADEDFILE_H
