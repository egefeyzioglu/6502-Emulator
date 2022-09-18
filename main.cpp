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

#define CEIL_DIVIDE_INT(x, y) x / y + (x % y > 0)
#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0')

// The emulator instance
Emulator *emulator;

int main(int argc, char *argv[]){

    QApplication prog(argc, argv);
    emulator = new Emulator();

    new MainWindow();

    return prog.exec();
}
