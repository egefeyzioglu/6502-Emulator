#include <QApplication>
#include <QTableWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QString>
#include <QHeaderView>
#include <QPushButton>
#include <QCommandLineParser>

#include <iostream>

#include "emulator.h"
#include "mainwindow.h"

// The emulator instance
Emulator *emulator;

// The main window instance
MainWindow *main_window;

const std::string kApplicationName = "6502 Emulator";
const std::string kApplicationVersion = "0.1";

int main(int argc, char *argv[]){
    // Create application object and set up command line arguments

    QApplication prog(argc, argv);
    QCoreApplication::setApplicationName(QString::fromStdString(kApplicationName));
    QCoreApplication::setApplicationVersion(QString::fromStdString(kApplicationVersion));

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("source", QCoreApplication::translate("file", "Source file to open."));

    parser.process(prog);

    QStringList args = parser.positionalArguments();

    // If we specified a file to open, remember to open it later

    std::string file_name_to_open = "";

    if(!args.empty()){
        file_name_to_open = args[0].toStdString();
    }

    // Create the emulator object first

    emulator = new Emulator();

    // Create the main window object

    main_window = new MainWindow(kApplicationName);

    // If we remembered a file to open, do so

    if(file_name_to_open.length() > 0){
        main_window->loadFile(file_name_to_open);
    }

    // Run the application
    return prog.exec();
}
