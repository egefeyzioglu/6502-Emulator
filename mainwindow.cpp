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
#include <QMessageBox>
#include <QProcess>
#include <QScrollBar>

#include <fstream>
#include <algorithm>
#include <filesystem>

#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "emulator.h"
#include "memorymodel.h"

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
void MainWindow::setUpMemoryTable(QTableView *&memory_view){
    constexpr int kMinimumHorizontalSize = 30;

    memory_view = new QTableView();
    memory_model = new MemoryModel(emulator->kMemorySize, emulator, this);
    memory_view -> setModel(memory_model);
    memory_view -> setShowGrid(true);
    memory_view -> setCornerButtonEnabled(false);
    memory_view -> setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectItems);
    memory_view -> setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
    memory_view -> horizontalHeader() -> setMinimumSectionSize(kMinimumHorizontalSize);
    memory_view -> resizeColumnsToContents();
    memory_view -> resizeRowsToContents();
}

/**
 * Updates the register view QTableWidget object, reads from emulator using getMemoryValue
 *
 * TODO: Don't destroy the QTableWidget every time
 *
 * @param register_table
 */
void MainWindow::updateRegisterTable(QTableWidget *&register_table){
    if(register_table == nullptr)
        register_table = new QTableWidget(6, 1);

    register_table -> clear();

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
void MainWindow::updateDockTitleBuildLog(bool isFloating){
    updateDockTitle(build_log_tab, isFloating);
}

// Menu slots

void MainWindow::handleMenuOpen(){
    // Create a file picker dialog
    openDialog = new QFileDialog(this);
    openDialog -> setNameFilter(tr("Assembly Files (*.s);;C Source Files (*.c);;C Header Files (*.h);;Any File(*)"));
    openDialog -> setViewMode(QFileDialog::Detail);
    QStringList fileNames;
    if (openDialog -> exec()){ // If the user picked a file,
        fileNames = openDialog -> selectedFiles();

        // Open and read the file into contents
        std::string contents;
        std::string line;

        std::ifstream newFile;
        newFile.open(fileNames[0].toStdString(), ios::in);

        while(getline(newFile, line)){
            contents += line + "\n";
        }

        // Create a new LoadedFile object to be inserted into loadedFiles
        LoadedFile newFileObj;

        // Break down the path to the file name and directory
        auto newFilePathObj = std::filesystem::path(fileNames[0] . toStdString());
        std::string newFileName = newFilePathObj.filename().u8string();
        std::string newFilePath = newFilePathObj.parent_path().u8string();

        // Populate the LoadedFile object and insert it into the loadedFiles vector
        newFileObj.fileName = QString::fromStdString(newFileName);
        newFileObj.fullPath = newFilePath + SLASH + newFileName;
        newFileObj.contents = QString::fromStdString(contents);
        newFileObj.savedSinceLastEdit = false;
        loadedFiles -> push_back(newFileObj);

        // Add the file as an option to fileDropdown
        fileDropdown -> addItem(fileNames[0]);
        fileDropdown -> setCurrentIndex(fileDropdown -> count() - 1);

        // Update the editor
        editor -> setPlainText(QString::fromUtf8(contents.data(), contents.length()));
        editor -> setDocumentTitle(fileNames[0]);

        // Update the editor title
        editorTitle -> setText(newFileObj.fileName);
    }
}

void MainWindow::handleMenuNew(){
    // The new file will be named "Untitled" then a number if there is already a file named that
    QString kNewFileName = "Untitled";
    QString kNewFileExtension = ".s";

    // The number of files open named the new file name
    int untitledCount = std::count_if(loadedFiles -> begin(), loadedFiles -> end(), [=](LoadedFile file){return file.fileName.startsWith(kNewFileName);});

    // Set the new document name to the new file name, then maybe a number
    editor -> setDocumentTitle(kNewFileName
                               + (untitledCount != 0 ?  QString("_") + QString::number(untitledCount) : "")
                               + kNewFileExtension);
    // Store new file name and contents
    LoadedFile newFileObj;
    newFileObj.fileName = editor -> documentTitle();
    newFileObj.fullPath = "";
    newFileObj.contents = QString(); // Assumed empty string since the editor still has the old contents at this point
    newFileObj.savedSinceLastEdit = false;
    loadedFiles -> push_back(newFileObj);

    // Add the file as an option to fileDropdown
    fileDropdown -> addItem(editor -> documentTitle());
    fileDropdown -> setCurrentIndex(fileDropdown -> count() - 1);

    // Clear the editor
    editor -> setPlainText(QString());
    // Trigger text change (since that doesn't happen automatically for some reason?)
    this -> updateOpenFileContents();

    // Update the editor title
    editorTitle -> setText(newFileObj.fileName);

}

void MainWindow::handleMenuSaveAs(){
    // Create a new file picker dialog
    openDialog = new QFileDialog(this);
    openDialog -> setNameFilter(tr("Assembly Files (*.s);;C Source Files (*.c);;C Header Files (*.h);;Any File(*)"));
    openDialog -> setViewMode(QFileDialog::Detail);
    QStringList fileNames;
    if (openDialog -> exec()){ // If the user picks a file
        fileNames = openDialog -> selectedFiles();

        // Open the file and write to it
        std::ofstream newFile;
        newFile.open(fileNames[0].toStdString(), ios::out);
        newFile << editor ->toPlainText().toStdString();

        // Update the editor title
        editor -> setDocumentTitle(fileNames[0]);

        // Get the current LoadedFile object from loadedFiles and update it
        LoadedFile &file = loadedFiles -> at(fileDropdown ->currentIndex());
        auto newFilePathObj = std::filesystem::path(fileNames[0] . toStdString());
        std::string newFileName = newFilePathObj.filename().u8string();
        file.fileName = QString::fromStdString(newFileName);
        file.fullPath = fileNames[0].toStdString();
        file.savedSinceLastEdit = true;

        // Update the fileDropdown item
        fileDropdown -> setItemText(fileDropdown ->currentIndex(), fileNames[0]);

        // Update the editor title
        editorTitle -> setText(file.fileName);

    }
}

void MainWindow::handleMenuSave(){
    // The fullPath property is never set (initialised to empty string) if we've never saved before
    // If so, treat this as Save As
    if(loadedFiles -> at(fileDropdown -> currentIndex()).fullPath == "") this -> handleMenuSaveAs();
    else{
        // Otherwise, open the file and write to it
        std::ofstream newFile;
        newFile.open(editor -> documentTitle().toStdString(), ios::out);
        newFile << editor ->toPlainText().toStdString();
        loadedFiles -> at(fileDropdown -> currentIndex()).savedSinceLastEdit = true;
    }
}

void MainWindow::handleMenuClose(){
    // If we don't have any files, return (this could happen if the user spams Ctrl + W)
    if(loadedFiles->size() == 0) return;

    // See if the current file was saved before the last edit
    if(loadedFiles -> at(fileDropdown -> currentIndex()).savedSinceLastEdit){ // If it has, delete it
        // Erase the loadedFiles object and remove item from dropdown (TODO: Check if file was saved and ask to confirm if it wasn't)
        loadedFiles->erase(loadedFiles->begin() + fileDropdown -> currentIndex());
        fileDropdown -> removeItem(fileDropdown -> currentIndex());

        // If we ran out of files, create a new one
        if(loadedFiles->size() == 0) handleMenuNew();

        // Update the editor title
        editorTitle -> setText(loadedFiles -> at(fileDropdown -> currentIndex()).fileName);
    }else{ // If it hasn't been saved, ask if the user wants to
        // Create a message box
        QMessageBox confirm;
        confirm.setText("Save file " + loadedFiles -> at(fileDropdown -> currentIndex()).fileName + "?");
        confirm.setIcon(QMessageBox::Question);
        confirm.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        confirm.setDefaultButton(QMessageBox::Save);
        int answer = confirm.exec();

        switch(answer){
        case QMessageBox::Save:     // If user wants to save,
            handleMenuSave();       // do so.
            // Call this function again (If the file was saved we won't get here)
            handleMenuClose();
            break;
        case QMessageBox::Discard:  // If the user doesn't want to save, don't save.
                                    // Pretend the file was saved
            loadedFiles -> at(fileDropdown -> currentIndex()).savedSinceLastEdit = true;
            // Call this function again (we marked the file as saved so we won't reach here)
            handleMenuClose();
            break;
        case QMessageBox::Cancel:   // If the user wants to cancel or
        default:                    // or if we got an answer we don't recognise,
            return;                 // do nothing
        }
    }
}

void MainWindow::updateOpenFileContents(){
    auto currentFile = this -> loadedFiles -> at(this -> fileDropdown -> currentIndex());
    currentFile.contents = this -> editor -> toPlainText();
    currentFile.savedSinceLastEdit = false;
}

void MainWindow::updateOpenFile(int selectedFileIndex){
    if(loadedFiles->size() > 0) this -> editor -> setPlainText(this -> loadedFiles -> at(selectedFileIndex).contents);
}

void MainWindow::emulatorStep(){
    emulator->step();
    updateRegisterTable(register_table);
}

void MainWindow::addToBuildLog(QString newContent){

    buildLog -> setPlainText(buildLog -> toPlainText() + "\n" + newContent);
    buildLog -> verticalScrollBar() -> setValue(buildLog -> verticalScrollBar() -> maximum());
}

void MainWindow::compileAndLoad(){
    Log::Info() << "Compiling and loading current file";

    // Compile/assmeble

    QString dasmPath = "C:\\Program Files\\dasm\\dasm.exe";
    QStringList dasmArgs;

    LoadedFile currentFile = loadedFiles -> at(fileDropdown->currentIndex());

    std::string outFileName = currentFile.fullPath.substr(0, currentFile.fullPath.find_last_of(".")) + ".tmp";

    dasmArgs << QString::fromUtf8(currentFile.fullPath);
    dasmArgs << "-v5" << "-f3" << QString::fromUtf8("-o" + outFileName);

    QProcess *compilerProcess = new QProcess();
    compilerProcess -> start(dasmPath, dasmArgs);
    if(!compilerProcess->waitForFinished()){
        Log::Warning() << "Timed out waiting for assembler";
    }

    QByteArray compilerOutput = compilerProcess -> readAll();
    addToBuildLog(QString::fromUtf8(compilerOutput.toStdString()));

    // Load

    // Creat ifstream and buffer to read into and read file
    std::ifstream outputFileInputStream(outFileName, std::ios::binary);
    char inBuf[emulator -> kMemorySize];
    if(!outputFileInputStream.read(inBuf, emulator -> kMemorySize) && !outputFileInputStream.eof()){
        Log::Warning() << "Could not read assembly output file when loading. rdstate = " << outputFileInputStream.rdstate();
        Log::Warning() << "eofbit = " << std::ifstream::eofbit << ", failbit = " << std::ifstream::failbit << ", badbit = " << std::ifstream::badbit << ", goodbit = " << std::ifstream::goodbit;
    }else{
        EmulatorHelper::replaceMemory((uint8_t*) inBuf, emulator -> kProgMemOffset, outputFileInputStream.gcount());
    }
    this -> resetEmulator();
    // Update memory view
    memory_model -> updateData();
}

void MainWindow::resetEmulator(){
    emulator -> resetCPU();
    updateRegisterTable(register_table);
}

void MainWindow::setUpEditor(QWidget *&editorContainer, QTextEdit *&editor, QLabel *&editorTitle){
    // Create the editor container, its layout, the editor, and editor title
    editorContainer = new QWidget();
    editorTitle = new QLabel();
    editor = new QTextEdit();
    QVBoxLayout *editorContainerLayout = new QVBoxLayout();

    // Add the widgets to the container layout
    editorContainerLayout -> addWidget(editorTitle);
    editorContainerLayout -> addWidget(editor);

    // Apply the layout to the container
    editorContainer -> setLayout(editorContainerLayout);

    // Create syntax highlighter, apply to the editor's document
    syntaxHighlighter = new SyntaxHighlighter(editor -> document());

    QFont font("Consolas");
    editor -> document() -> setDefaultFont(font);

    this -> connect(editor, &QTextEdit::textChanged, this, &MainWindow::updateOpenFileContents);

}

void MainWindow::setUpBuildLog(QPlainTextEdit *&compilerLog){
    compilerLog = new QPlainTextEdit();
    compilerLog -> setTextInteractionFlags(Qt::TextBrowserInteraction | Qt::TextSelectableByKeyboard);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    this -> setWindowTitle(kWindowTitle.data());
    this -> setWindowIcon(QIcon(":/resources/img/icon.xpm"));
    this -> resize(1200,600);

    // Set up the base widgets
    setUpMemoryTable(memory_view);
    updateRegisterTable(register_table);
    QVBoxLayout *editorContainerLayout;
    setUpEditor(editorContainer, editor, editorTitle);
    setUpBuildLog(buildLog);

    step_button = new QPushButton("Step");

    // Set up the dock widgets
    register_tab = new QDockWidget("Registers");
    memory_tab = new QDockWidget("Memory");
    emulator_controls_tab = new QDockWidget("Controls");
    build_log_tab = new QDockWidget("Build Log");

    emulator_controls_tab -> setWidget(step_button);
    memory_tab -> setWidget(memory_view);
    register_tab -> setWidget(register_table);
    build_log_tab -> setWidget(buildLog);


    // Dock widget title updates
    connect(memory_tab, &QDockWidget::topLevelChanged, this, &MainWindow::updateDockTitleMemory);
    connect(register_tab, &QDockWidget::topLevelChanged, this, &MainWindow::updateDockTitleRegisters);
    connect(emulator_controls_tab, &QDockWidget::topLevelChanged, this, &MainWindow::updateDockTitleControls);
    connect(build_log_tab, &QDockWidget::topLevelChanged, this, &MainWindow::updateDockTitleBuildLog);

    // Set up menus

    newAction = new QAction("New File");
    openAction = new QAction("Open");
    saveAction = new QAction("Save");
    saveAsAction = new QAction("Save As");
    closeAction = new QAction("Close");

    buildAction = new QAction(tr("&Build"));

    newAction -> setShortcut(Qt::CTRL | Qt::Key_N);
    openAction -> setShortcut(Qt::CTRL | Qt::Key_O);
    saveAction -> setShortcut(Qt::CTRL | Qt::Key_S);
    saveAsAction -> setShortcut(Qt::CTRL | Qt::SHIFT | Qt::Key_S);
    closeAction -> setShortcut(Qt::CTRL | Qt::Key_W);
    buildAction -> setShortcut(Qt::Key_F5);

    fileMenu = menuBar() -> addMenu(tr("&File"));
    buildMenu = menuBar() -> addMenu(tr("&Build"));

    fileMenu->addAction(newAction);
    fileMenu->addAction(openAction);
    fileMenu->addAction(saveAction);
    fileMenu->addAction(saveAsAction);
    fileMenu->addAction(closeAction);

    buildMenu -> addAction(buildAction);

    connect(newAction, &QAction::triggered, this, &MainWindow::handleMenuNew);
    connect(openAction, &QAction::triggered, this, &MainWindow::handleMenuOpen);
    connect(saveAsAction, &QAction::triggered, this, &MainWindow::handleMenuSaveAs);
    connect(saveAction, &QAction::triggered, this, &MainWindow::handleMenuSave);
    connect(closeAction, &QAction::triggered, this, &MainWindow::handleMenuClose);

    connect(buildAction, &QAction::triggered, this, &MainWindow::compileAndLoad);

    // Toolbar
    toolBar = new QToolBar();
    fileDropdown =  new QComboBox();
    fileDropdown -> setSizeAdjustPolicy(QComboBox::AdjustToContents);
    fileDropdownAction = toolBar -> addWidget(fileDropdown);

    loadedFiles = new std::vector<LoadedFile>;

    connect(fileDropdown, &QComboBox::currentIndexChanged, this, &MainWindow::updateOpenFile);


    // Emulator controls
    connect(step_button, &QPushButton::clicked, this, &MainWindow::emulatorStep);

    // Add everything to the window
    this -> setCentralWidget(editorContainer);
    this -> addDockWidget(Qt::LeftDockWidgetArea, memory_tab);
    this -> addDockWidget(Qt::RightDockWidgetArea, register_tab);
    this -> addDockWidget(Qt::RightDockWidgetArea, emulator_controls_tab);
    this -> addDockWidget(Qt::BottomDockWidgetArea, build_log_tab);
    this -> addToolBar(toolBar);

    // Set the size for the build log tab
    this -> resizeDocks({build_log_tab}, {kBuildLogDefaultSize}, Qt::Vertical);

    this -> show();

    // Start with a blank document
    handleMenuNew();
}

MainWindow::~MainWindow() {
    delete memory_view;
    delete memory_model;
    delete register_table;
    delete step_button;

    delete editor;
    delete editorContainer;
    delete editorTitle;

    delete buildLog;

    delete build_button;

    delete memory_tab;
    delete register_tab;
    delete emulator_controls_tab;
    delete build_log_tab;

    delete fileMenu;
    delete buildMenu;

    delete saveAction;
    delete saveAsAction;
    delete newAction;
    delete openAction;
    delete closeAction;
    delete buildAction;

    delete toolBar;
    delete fileDropdown;
    delete fileDropdownAction;

    delete loadedFiles;

    delete openDialog;

    delete syntaxHighlighter;

    delete ui;
}

