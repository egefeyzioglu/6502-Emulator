#ifndef LOADEDFILE_H
#define LOADEDFILE_H

#include <QString>

#ifdef _WIN32
    #define SLASH "\\"
#else
    #define SLASH "/"
#endif

/**
 * Represents a file loaded into the editor
 */
class LoadedFile
{
public:
    /**
     * The contents of the file
     */
    QString contents = "";
    /**
     * The name of the file, including the extension but not including
     * the parent directory
     */
    QString file_name = "";
    /**
     * The full path to the file, including the name, extension, and the
     * parent directory
     */
    std::string full_path = "";
    /**
     * Whether this file has been saved since the last time it was edited
     */
    bool saved_since_last_edit = false;
};

#endif // LOADEDFILE_H
