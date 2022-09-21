#include <QApplication>
#include <QTableWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QString>
#include <QHeaderView>
#include <QPushButton>

#include <iostream>

#include "emulator.h"
#include "mainwindow.h"

// The emulator instance
Emulator *emulator;

int main(int argc, char *argv[]){

    QApplication prog(argc, argv);
    emulator = new Emulator();

    new MainWindow();

    return prog.exec();
}
