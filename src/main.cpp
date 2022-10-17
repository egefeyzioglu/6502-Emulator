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
MainWindow *mainWindow;

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

    std::string fileNameToOpen = "";

    if(!args.empty()){
        fileNameToOpen = args[0].toStdString();
    }

    // Create the emulator object first

    emulator = new Emulator();

    // Create the main window object

    mainWindow = new MainWindow(kApplicationName);

    // If we remembered a file to open, do so

    if(fileNameToOpen.length() > 0){
        mainWindow->loadFile(fileNameToOpen);
    }

    // Run the application
    return prog.exec();
}
