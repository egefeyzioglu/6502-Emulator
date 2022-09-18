#include <QTableWidget>
#include <QHBoxLayout>
#include <QApplication>
#include <QLabel>
#include <QString>
#include <QHeaderView>
#include <QPushButton>
#include <QDockWidget>
#include <QFontDialog>
#include <QShortcut>

#include <fstream>

#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "emulator.h"
#include "log.h"

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

// The global emulator instance
extern Emulator *emulator;

/**
 * Updates the memory view QTableWidget object, reads from emulator using getMemoryValue
 *
 * TODO: Don't destroy the QTableWidget every time
 *
 * @param memory_table
 */
void MainWindow::updateMemoryTable(QTableWidget *&memory_table){
    delete memory_table; // Deleting nullptr is safe
    const int kRowCount = CEIL_DIVIDE_INT(emulator -> kMemorySize, 0x10);
    memory_table = new QTableWidget(kRowCount, 0x10);
    memory_table->horizontalHeader() -> setSectionResizeMode(QHeaderView::ResizeToContents);
    for(int row = 0; row < kRowCount; row++){
        for(int col = 0; col < 0x10; col++){
            QLabel *cellLabel = new QLabel;
            QString newLabelString = QString();

            newLabelString = QString::asprintf("%02x", emulator -> getMemoryValue(row * 0x10 + col));
            cellLabel -> setText(newLabelString);

            newLabelString = QString::asprintf("%04x", row * 0x10);
            memory_table -> setVerticalHeaderItem(row, new QTableWidgetItem(newLabelString));

            memory_table -> setCellWidget(row, col, cellLabel);
        }
    }
}

/**
 * Updates the register view QTableWidget object, reads from emulator using getMemoryValue
 *
 * TODO: Don't destroy the QTableWidget every time
 *
 * @param register_table
 */
void MainWindow::updateRegisterTable(QTableWidget *&register_table){
    delete register_table; // Deleting nullptr is safe
    register_table = new QTableWidget(6, 1);

    // Set up titles

    // Register A title
    QTableWidgetItem *header_reg_A = new QTableWidgetItem();
    header_reg_A->setText("A");
    register_table -> setVerticalHeaderItem(0,header_reg_A);
    // Register X title
    QTableWidgetItem *header_reg_X = new QTableWidgetItem();
    header_reg_X->setText("X");
    register_table -> setVerticalHeaderItem(1,header_reg_X);
    // Register Y title
    QTableWidgetItem *header_reg_Y = new QTableWidgetItem();
    header_reg_Y->setText("Y");
    register_table -> setVerticalHeaderItem(2,header_reg_Y);
    // Register SP title
    QTableWidgetItem *header_reg_SP = new QTableWidgetItem();
    header_reg_SP->setText("SP");
    register_table -> setVerticalHeaderItem(3,header_reg_SP);
    // Register PC title
    QTableWidgetItem *header_reg_PC = new QTableWidgetItem();
    header_reg_PC->setText("PC");
    register_table -> setVerticalHeaderItem(4,header_reg_PC);
    // Register status title
    QTableWidgetItem *header_reg_status = new QTableWidgetItem();
    header_reg_status->setText("Status");
    register_table -> setVerticalHeaderItem(5,header_reg_status);
    // Horizontal title
    QTableWidgetItem *header_horizontal = new QTableWidgetItem();
    header_horizontal->setText("");
    register_table -> setHorizontalHeaderItem(0,header_horizontal);

    // Set register values

    // Register A
    QTableWidgetItem *reg_A_value = new QTableWidgetItem(QString::asprintf("%c%c%c%c%c%c%c%c (0x%02x)" , BYTE_TO_BINARY(emulator->get6502()->GetA()), emulator->get6502()->GetA()));
    register_table -> setItem(0,0, reg_A_value);
    // Register X
    QTableWidgetItem *reg_X_value = new QTableWidgetItem(QString::asprintf("%c%c%c%c%c%c%c%c (0x%02x)" , BYTE_TO_BINARY(emulator->get6502()->GetX()), emulator->get6502()->GetX()));
    register_table -> setItem(1,0, reg_X_value);
    // Register Y
    QTableWidgetItem *reg_Y_value = new QTableWidgetItem(QString::asprintf("%c%c%c%c%c%c%c%c (0x%02x)" , BYTE_TO_BINARY(emulator->get6502()->GetY()), emulator->get6502()->GetY()));
    register_table -> setItem(2,0, reg_Y_value);
    // Register SP
    QTableWidgetItem *reg_SP_value = new QTableWidgetItem(QString::asprintf("%c%c%c%c%c%c%c%c (0x%02x)" , BYTE_TO_BINARY(emulator->get6502()->GetS()), emulator->get6502()->GetS()));
    register_table -> setItem(3,0, reg_SP_value);
    // Register PC
    QTableWidgetItem *reg_PC_value = new QTableWidgetItem(QString::asprintf("%c%c%c%c%c%c%c%c (0x%02x)" , BYTE_TO_BINARY(emulator->get6502()->GetPC()), emulator->get6502()->GetPC()));
    register_table -> setItem(4,0, reg_PC_value);
    // Register status
    QTableWidgetItem *reg_status_value = new QTableWidgetItem(QString::asprintf("%c%c%c%c%c%c%c%c (0x%02x)" , BYTE_TO_BINARY(emulator->get6502()->GetP()), emulator->get6502()->GetP()));
    register_table -> setItem(5, 0, reg_status_value);
}

/**
 * Updates the QDockWidget title depending on whether it is docked or not
 *
 * @param dockWidget the dock widget
 * @param isFloating is this widget floating
 */
void MainWindow::updateDockTitle(QDockWidget  *dockWidget, bool isFloating){

    // Seperator between base title and kWindowTitle
    QString seperator = " | ";

    if(isFloating){
        // Add kWindowTitle and seperator to the tab title
        dockWidget -> setWindowTitle(
                    dockWidget -> windowTitle() +
                    seperator +
                    QString::fromUtf8( this -> kWindowTitle.data(), this -> kWindowTitle.size())
                    );
    }else{
        // Remove kWindowTitle and seperator from the tab title
        dockWidget -> setWindowTitle(
                    dockWidget -> windowTitle().first(dockWidget -> windowTitle().size() - (seperator.size() + this -> kWindowTitle.length()))
                    );
    }
}

// Slots for when the dock widgets start and stop floating

void MainWindow::updateDockTitleMemory(bool isFloating){
    updateDockTitle(memory_tab, isFloating);
}
void MainWindow::updateDockTitleRegisters(bool isFloating){
    updateDockTitle(register_tab, isFloating);
}
void MainWindow::updateDockTitleControls(bool isFloating){
    updateDockTitle(emulator_controls_tab, isFloating);
}

// Menu slots

void MainWindow::handleMenuOpen(){
    openDialog = new QFileDialog(this);
    openDialog -> setNameFilter(tr("Assembly Files (*.s);;C Source Files (*.c);;C Header Files (*.h);;Any File(*)"));
    openDialog -> setViewMode(QFileDialog::Detail);
    QStringList fileNames;
    if (openDialog -> exec()){
        fileNames = openDialog -> selectedFiles();

        std::string contents;
        std::string line;

        std::ifstream newFile;
        newFile.open(fileNames[0].toStdString(), ios::in);

        while(getline(newFile, line)){
            contents += line + "\n";
        }

        editor -> setPlainText(QString::fromUtf8(contents.data(), contents.length()));
        editor -> setDocumentTitle(fileNames[0]);
    }
}

void MainWindow::handleMenuNew(){
    editor -> setPlainText(QString());
    editor -> setDocumentTitle("");
}

void MainWindow::handleMenuSaveAs(){
    openDialog = new QFileDialog(this);
    openDialog -> setNameFilter(tr("Assembly Files (*.s);;C Source Files (*.c);;C Header Files (*.h);;Any File(*)"));
    openDialog -> setViewMode(QFileDialog::Detail);
    QStringList fileNames;
    if (openDialog -> exec()){
        fileNames = openDialog -> selectedFiles();

        std::ofstream newFile;
        newFile.open(fileNames[0].toStdString(), ios::out);
        newFile << editor ->toPlainText().toStdString();

        editor -> setDocumentTitle(fileNames[0]);
    }
}

void MainWindow::handleMenuSave(){
    if(editor -> documentTitle() == "") this -> handleMenuSaveAs();
    else{

        std::ofstream newFile;
        newFile.open(editor -> documentTitle().toStdString(), ios::out);
        newFile << editor ->toPlainText().toStdString();
    }
}


/**
 * Slot for the emulator step button
 *
 */
void MainWindow::emulatorStep(){
    Log::Info() << "Step clicked";
    emulator->step();
}

void MainWindow::setUpEditor(QPlainTextEdit *&editor){
    editor = new QPlainTextEdit();

    QFont font("Consolas");
    QTextCharFormat format;
    format.setFont(font);
    editor->mergeCurrentCharFormat(format);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    this -> setWindowTitle(kWindowTitle.data());
    this -> setWindowIcon(QIcon(":/resources/img/icon.xpm"));
    this -> resize(1200,600);

    // Set up the base widgets
    updateMemoryTable(memory_table); // TODO: Change this to a QDataWidgetMapper loading data from a QAbstractItemModel
    updateRegisterTable(register_table);
    setUpEditor(editor);

    step_button = new QPushButton("Step");

    // Set up the dock widgets
    register_tab = new QDockWidget("Registers");
    memory_tab = new QDockWidget("Memory");
    emulator_controls_tab = new QDockWidget("Controls");

    emulator_controls_tab -> setWidget(step_button);
    memory_tab -> setWidget(memory_table);
    register_tab -> setWidget(register_table);

    // Dock widget title updates
    connect(memory_tab, &QDockWidget::topLevelChanged, this, &MainWindow::updateDockTitleMemory);
    connect(register_tab, &QDockWidget::topLevelChanged, this, &MainWindow::updateDockTitleRegisters);
    connect(emulator_controls_tab, &QDockWidget::topLevelChanged, this, &MainWindow::updateDockTitleControls);

    // Set up menus

    newAction = new QAction("New File");
    openAction = new QAction("Open");
    saveAction = new QAction("Save");
    saveAsAction = new QAction("Save As");

    newAction -> setShortcut(Qt::CTRL | Qt::Key_N);
    openAction -> setShortcut(Qt::CTRL | Qt::Key_O);
    saveAction -> setShortcut(Qt::CTRL | Qt::Key_S);
    saveAsAction -> setShortcut(Qt::CTRL | Qt::SHIFT | Qt::Key_S);

    fileMenu = menuBar()->addMenu(tr("&File"));

    fileMenu->addAction(newAction);
    fileMenu->addAction(openAction);
    fileMenu->addAction(saveAction);
    fileMenu->addAction(saveAsAction);

    connect(newAction, &QAction::triggered, this, &MainWindow::handleMenuNew);
    connect(openAction, &QAction::triggered, this, &MainWindow::handleMenuOpen);
    connect(saveAsAction, &QAction::triggered, this, &MainWindow::handleMenuSaveAs);
    connect(saveAction, &QAction::triggered, this, &MainWindow::handleMenuSave);

    // Emulator controls
    connect(step_button, &QPushButton::clicked, this, &MainWindow::emulatorStep);

    // Add everything to the window
    this -> setCentralWidget(editor);
    this -> addDockWidget(Qt::LeftDockWidgetArea, memory_tab);
    this -> addDockWidget(Qt::RightDockWidgetArea, register_tab);
    this -> addDockWidget(Qt::RightDockWidgetArea, emulator_controls_tab);

    this -> show();
}

MainWindow::~MainWindow()
{
    delete memory_table;
    delete register_table;
    delete step_button;
    delete editor;

    delete memory_tab;
    delete register_tab;
    delete emulator_controls_tab;

    delete saveAction;
    delete saveAsAction;
    delete newAction;
    delete openAction;
    delete fileMenu;

    delete openDialog;

    delete ui;
}

