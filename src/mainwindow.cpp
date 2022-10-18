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
#include <QSpinBox>
#include <QTimer>
#include <QLineEdit>

#include <fstream>
#include <algorithm>
#include <filesystem>

#include "mainwindow.h"
#include "ui_mainwindow.h"
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

void MainWindow::setUpMemoryTable(QTableView *&memory_view){
    // Create and format the memory table

    constexpr int kMinimumHorizontalSize = 30;

    memory_view = new QTableView(); // TODO: Don't leak memory on subsequent calls
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

void MainWindow::updateRegisterTable(QTableWidget *&register_table){

    // Register format strings
    QString reg_A_value_string = QString::asprintf("%c%c%c%c%c%c%c%c (0x%02x)" , BYTE_TO_BINARY(emulator->get6502()->GetA()), emulator->get6502()->GetA());
    QString reg_X_value_string = QString::asprintf("%c%c%c%c%c%c%c%c (0x%02x)" , BYTE_TO_BINARY(emulator->get6502()->GetX()), emulator->get6502()->GetX());
    QString reg_Y_value_string = QString::asprintf("%c%c%c%c%c%c%c%c (0x%02x)" , BYTE_TO_BINARY(emulator->get6502()->GetY()), emulator->get6502()->GetY());
    QString reg_SP_value_string = QString::asprintf("%c%c%c%c%c%c%c%c (0x%02x)" , BYTE_TO_BINARY(emulator->get6502()->GetP()), emulator->get6502()->GetP());
    QString reg_PC_value_string = QString::asprintf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c (0x%04x)" ,
                                                    BYTE_TO_BINARY(emulator->get6502()->GetPC() % 0xFF00),
                                                    BYTE_TO_BINARY(emulator->get6502()->GetPC() % 0x00FF),
                                                    emulator->get6502()->GetPC());
    QString reg_status_value_string = QString::asprintf("%c%c%c%c%c%c%c%c (0x%02x)" , BYTE_TO_BINARY(emulator->get6502()->GetPC()), emulator->get6502()->GetPC());


    if(register_table == nullptr){
        // If the table doesn't exist, create it and set it up

        register_table = new QTableWidget(6, 1);
        connect(emulator, &Emulator::registersChanged, this, &MainWindow::handleRegistersChanged);

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
        QTableWidgetItem *reg_A_value = new QTableWidgetItem(reg_A_value_string);
        register_table -> setItem(0,0, reg_A_value);
        // Register X
        QTableWidgetItem *reg_X_value = new QTableWidgetItem(reg_X_value_string);
        register_table -> setItem(1,0, reg_X_value);
        // Register Y
        QTableWidgetItem *reg_Y_value = new QTableWidgetItem(reg_Y_value_string);
        register_table -> setItem(2,0, reg_Y_value);
        // Register SP
        QTableWidgetItem *reg_SP_value = new QTableWidgetItem(reg_SP_value_string);
        register_table -> setItem(3,0, reg_SP_value);
        // Register PC
        QTableWidgetItem *reg_PC_value = new QTableWidgetItem(reg_PC_value_string);
        register_table -> setItem(4,0, reg_PC_value);
        // Register status
        QTableWidgetItem *reg_status_value = new QTableWidgetItem(reg_status_value_string);
        register_table -> setItem(5, 0, reg_status_value);
    } else {
        // If the table already exists, just update the register values
        // Register A
        register_table -> item(0,0) -> setText(reg_A_value_string);
        // Register X
        register_table -> item(1,0) -> setText(reg_X_value_string);
        // Register Y
        register_table -> item(2,0) -> setText(reg_Y_value_string);
        // Register S
        register_table -> item(3,0) -> setText(reg_SP_value_string);
        // Register PC
        register_table -> item(4,0) -> setText(reg_PC_value_string);
        // Register status
        register_table -> item(5,0) -> setText(reg_status_value_string);
    }

    // Align width to contents (need to fit the program counter)
    register_table -> resizeColumnsToContents();
}

void MainWindow::updateDockTitle(QDockWidget  *dock_widget, bool is_floating){

    // Seperator between base title and kWindowTitle
    QString seperator = " | ";

    if(is_floating){
        // Add kWindowTitle and seperator to the tab title
        dock_widget -> setWindowTitle(
                    dock_widget -> windowTitle() +
                    seperator +
                    QString::fromUtf8( this -> kWindowTitle.data(), this -> kWindowTitle.size())
                    );
    }else{
        // Remove kWindowTitle and seperator from the tab title
        dock_widget -> setWindowTitle(
                    dock_widget -> windowTitle().first(dock_widget -> windowTitle().size() - (seperator.size() + this -> kWindowTitle.length()))
                    );
    }
}

// Slots for when the dock widgets start and stop floating

void MainWindow::updateDockTitleMemory(bool is_floating){
    updateDockTitle(memory_tab, is_floating);
}
void MainWindow::updateDockTitleRegisters(bool is_floating){
    updateDockTitle(register_tab, is_floating);
}
void MainWindow::updateDockTitleControls(bool is_floating){
    updateDockTitle(emulator_controls_tab, is_floating);
}
void MainWindow::updateDockTitleBuildLog(bool is_floating){
    updateDockTitle(build_log_tab, is_floating);
}

// Menu slots

void MainWindow::handleMenuOpen(){
    // Create a file picker dialog
    open_dialog = new QFileDialog(this);
    open_dialog -> setNameFilter(tr("Assembly Files (*.s);;C Source Files (*.c);;C Header Files (*.h);;Any File(*)"));
    open_dialog -> setViewMode(QFileDialog::Detail);
    QStringList file_names;
    if (open_dialog -> exec()){ // If the user picked a file,
        // Load the file
        loadFile(open_dialog -> selectedFiles()[0].toStdString());
    }
}

void MainWindow::handleMenuNew(){
    // The new file will be named "Untitled" then a number if there is already a file named that
    QString kNewFileName = "Untitled";
    QString kNewFileExtension = ".s";

    // The number of files open named the new file name
    int untitled_count = std::count_if(loaded_files -> begin(), loaded_files -> end(), [=](LoadedFile file){return file.file_name.startsWith(kNewFileName);});

    // Set the new document name to the new file name, then maybe a number
    editor -> setDocumentTitle(kNewFileName
                               + (untitled_count != 0 ?  QString("_") + QString::number(untitled_count) : "")
                               + kNewFileExtension);
    // Store new file name and contents
    LoadedFile new_file_obj;
    new_file_obj.file_name = editor -> documentTitle();
    new_file_obj.full_path = "";
    new_file_obj.contents = QString(); // Assumed empty string since the editor still has the old contents at this point
    new_file_obj.saved_since_last_edit = false;
    loaded_files -> push_back(new_file_obj);

    // Add the file as an option to fileDropdown
    file_dropdown -> addItem(editor -> documentTitle());
    file_dropdown -> setCurrentIndex(file_dropdown -> count() - 1);

    // Clear the editor
    editor -> setPlainText(QString());
    // Trigger text change (since that doesn't happen automatically for some reason?)
    this -> updateOpenFileContents();

    // Update the editor title
    editor_title -> setText(new_file_obj.file_name);

}

void MainWindow::handleMenuSaveAs(){
    // Create a new file picker dialog
    open_dialog = new QFileDialog(this);
    open_dialog -> setNameFilter(tr("Assembly Files (*.s);;C Source Files (*.c);;C Header Files (*.h);;Any File(*)"));
    open_dialog -> setViewMode(QFileDialog::Detail);
    open_dialog -> setAcceptMode(QFileDialog::AcceptSave);
    QStringList file_names;
    if (open_dialog -> exec()){ // If the user picks a file
        file_names = open_dialog -> selectedFiles();

        // Open the file and write to it
        std::ofstream new_file;
        new_file.open(file_names[0].toStdString(), ios::out);
        new_file << editor ->toPlainText().toStdString();

        // Update the editor title
        editor -> setDocumentTitle(file_names[0]);

        // Get the current LoadedFile object from loadedFiles and update it
        LoadedFile &file = loaded_files -> at(file_dropdown ->currentIndex());
        auto new_file_path_obj = std::filesystem::path(file_names[0] . toStdString());
        std::string new_file_name = new_file_path_obj.filename().u8string();
        file.file_name = QString::fromStdString(new_file_name);
        file.full_path = file_names[0].toStdString();
        file.saved_since_last_edit = true;

        // Update the fileDropdown item
        file_dropdown -> setItemText(file_dropdown ->currentIndex(), file_names[0]);

        // Update the editor title
        editor_title -> setText(file.file_name);

    }
}

void MainWindow::handleMenuSave(){
    // The fullPath property is never set (initialised to empty string) if we've never saved before
    // If so, treat this as Save As
    if(loaded_files -> at(file_dropdown -> currentIndex()).full_path == "") this -> handleMenuSaveAs();
    else{
        // Otherwise, open the file and write to it
        std::ofstream new_file;
        new_file.open(editor -> documentTitle().toStdString(), ios::out);
        new_file << editor ->toPlainText().toStdString();
        loaded_files -> at(file_dropdown -> currentIndex()).saved_since_last_edit = true;
    }
}

void MainWindow::handleMenuClose(){
    // If we don't have any files, return (this could happen if the user spams Ctrl + W)
    if(loaded_files->size() == 0) return;

    // See if the current file was saved before the last edit
    if(loaded_files -> at(file_dropdown -> currentIndex()).saved_since_last_edit){ // If it has, delete it
        // Erase the loadedFiles object and remove item from dropdown (TODO: Check if file was saved and ask to confirm if it wasn't)
        loaded_files->erase(loaded_files->begin() + file_dropdown -> currentIndex());
        file_dropdown -> removeItem(file_dropdown -> currentIndex());

        // If we ran out of files, create a new one
        if(loaded_files->size() == 0) handleMenuNew();

        // Update the editor title
        editor_title -> setText(loaded_files -> at(file_dropdown -> currentIndex()).file_name);
    }else{ // If it hasn't been saved, ask if the user wants to
        // Create a message box
        QMessageBox confirm;
        confirm.setText("Save file " + loaded_files -> at(file_dropdown -> currentIndex()).file_name + "?");
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
            loaded_files -> at(file_dropdown -> currentIndex()).saved_since_last_edit = true;
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
    // Get the current file object, update its contents to the contents of the editor, and mark it as unsaved
    auto current_file = this -> loaded_files -> at(this -> file_dropdown -> currentIndex());
    current_file.contents = this -> editor -> toPlainText();
    current_file.saved_since_last_edit = false;
}

void MainWindow::updateOpenFile(int selectedFileIndex){
    // Get the requested file object and set the contents of the editor to its contents
    if(loaded_files->size() > 0) this -> editor -> setPlainText(this -> loaded_files -> at(selectedFileIndex).contents);
}

void MainWindow::handleRegistersChanged(std::vector<Emulator::Register> registers_to_update){
    // Clear old highlighting
    for(int row = 0; row < register_table -> rowCount(); row++){
        register_table -> item(row,0) -> setBackground(QBrush(Qt::white));
    }
    // Iterate over the list of updated registers and highlight them
    for(Emulator::Register reg: registers_to_update){
        switch(reg){
        case Emulator::Register::A:
            register_table -> item(0,0) -> setBackground(QBrush(Qt::yellow));
            break;
        case Emulator::Register::X:
            register_table -> item(1,0) -> setBackground(QBrush(Qt::yellow));
            break;
        case Emulator::Register::Y:
            register_table -> item(2,0) -> setBackground(QBrush(Qt::yellow));
            break;
        case Emulator::Register::P:
            register_table -> item(3,0) -> setBackground(QBrush(Qt::yellow));
            break;
        case Emulator::Register::PC:
            register_table -> item(4,0) -> setBackground(QBrush(Qt::yellow));
            break;
        case Emulator::Register::S:
            register_table -> item(5,0) -> setBackground(QBrush(Qt::yellow));
            break;
        }
    }
}

void MainWindow::emulatorStep(){
    emulator->step();
    // The register table must be manually updated since it is a default QTableWidget
    updateRegisterTable(register_table);
}

void MainWindow::addToBuildLog(QString newContent){
    // Append to build log then scroll it to the very bottom
    build_log -> setPlainText(build_log -> toPlainText() + "\n" + newContent);
    build_log -> verticalScrollBar() -> setValue(build_log -> verticalScrollBar() -> maximum());
}

void MainWindow::interruptEmulator(){
    emulator -> interrupt();
}

void MainWindow::compileAndLoad(){
    // Check if we saved this file, prompt to save if not
    if(!loaded_files -> at(file_dropdown -> currentIndex()).saved_since_last_edit){
        // Create a message box
        QMessageBox confirm;
        confirm.setText("Save file " + loaded_files -> at(file_dropdown -> currentIndex()).file_name + "?");
        confirm.setIcon(QMessageBox::Question);
        confirm.setStandardButtons(QMessageBox::Save | QMessageBox::Cancel);
        confirm.setDefaultButton(QMessageBox::Save);
        // Ask to confirm
        int answer = confirm.exec();
        switch(answer){
        case QMessageBox::Save:
            // If the user wants to save, save
            handleMenuSave();
            // Then call this function again (If the file was saved we won't get here)
            compileAndLoad();
            return;
        case QMessageBox::Cancel:
            // If the user doesn't, cancel
            return;
        }
    }

    Log::Info() << "Compiling and loading current file";

    // Compile/assmeble

    QString dasm_path = "C:\\Program Files\\dasm\\dasm.exe";
    QStringList dasm_args;

    LoadedFile current_file = loaded_files -> at(file_dropdown->currentIndex());

    std::string out_file_name = current_file.full_path.substr(0, current_file.full_path.find_last_of(".")) + ".tmp";

    dasm_args << QString::fromUtf8(current_file.full_path);
    dasm_args << "-v5" << "-f3" << QString::fromUtf8("-o" + out_file_name);

    QProcess *compiler_process = new QProcess();
    compiler_process -> start(dasm_path, dasm_args);
    if(!compiler_process->waitForFinished()){
        Log::Warning() << "Timed out waiting for assembler";
    }

    QByteArray compiler_output = compiler_process -> readAll();
    addToBuildLog(QString::fromUtf8(compiler_output.toStdString()));

    // Load

    // Creat ifstream and buffer to read into and read file
    std::ifstream output_file_input_stream(out_file_name, std::ios::binary);
    char inBuf[emulator -> kMemorySize];
    if(!output_file_input_stream.read(inBuf, emulator -> kMemorySize) && !output_file_input_stream.eof()){
        Log::Warning() << "Could not read assembly output file when loading. rdstate = " << output_file_input_stream.rdstate();
        Log::Warning() << "eofbit = " << std::ifstream::eofbit << ", failbit = " << std::ifstream::failbit << ", badbit = " << std::ifstream::badbit << ", goodbit = " << std::ifstream::goodbit;
    }else{
        EmulatorHelper::replaceMemory((uint8_t*) inBuf, emulator -> kProgMemOffset, output_file_input_stream.gcount());
    }
    this -> resetEmulator();
    // Update memory view
    memory_model -> updateData();
}

void MainWindow::resetEmulator(){
    emulator -> resetCPU();
    // The register table must be manually updated since it is a default QTableWidget
    updateRegisterTable(register_table);
}

void MainWindow::setUpEditor(QWidget *&editor_container, QTextEdit *&editor, QLabel *&editor_title){
    // Create the editor container, its layout, the editor, and editor title
    editor_container = new QWidget();
    editor_title = new QLabel();
    editor = new QTextEdit();
    QVBoxLayout *editor_container_layout = new QVBoxLayout();

    // Add the widgets to the container layout
    editor_container_layout -> addWidget(editor_title);
    editor_container_layout -> addWidget(editor);

    // Apply the layout to the container
    editor_container -> setLayout(editor_container_layout);

    // Create syntax highlighter, apply to the editor's document
    syntax_highlighter = new SyntaxHighlighter(editor -> document());

    QFont font("Consolas");
    editor -> document() -> setDefaultFont(font);

    this -> connect(editor, &QTextEdit::textChanged, this, &MainWindow::updateOpenFileContents);

}

void MainWindow::setUpBuildLog(QPlainTextEdit *&compiler_log){
    // Create a new plain text editor, disallow editing but allow everything else
    compiler_log = new QPlainTextEdit();
    compiler_log -> setTextInteractionFlags(Qt::TextBrowserInteraction | Qt::TextSelectableByKeyboard);
}

void MainWindow::setUpEmulatorControls(QWidget *&emulator_controls_wrapper){
    // Create the emulator controls
    step_button = new QPushButton(tr("Step"));
    run_button = new QPushButton(tr("Run"));
    interrupt_button = new QPushButton(tr("Interrupt"));
    clock_speed_label = new QLabel(tr("Target Clock Speed"));
    clock_speed_value = new QLineEdit();
    real_clock_speed_label = new QLabel(tr("Actual Clock Speed"));
    real_clock_speed_value = new QLabel(tr("Stopped"));

    // Create the wrapper and the layout
    QGridLayout *emulator_controls_layout = new QGridLayout();
    emulator_controls_wrapper = new QWidget();

    // Add the controls to the layout, and add the layout to the wrapper
    emulator_controls_layout -> addWidget(step_button, 0, 0, 1, 1);
    emulator_controls_layout -> addWidget(run_button, 0, 1, 1, 1);
    emulator_controls_layout -> addWidget(interrupt_button, 0, 2, 1, 1);
    emulator_controls_layout -> addWidget(clock_speed_label, 1, 0, 1, 1);
    emulator_controls_layout -> addWidget(clock_speed_value, 1, 1, 1, 2);
    emulator_controls_layout -> addWidget(real_clock_speed_label, 2, 0, 1, 1);
    emulator_controls_layout -> addWidget(real_clock_speed_value, 2, 1, 1, 2);

    emulator_controls_wrapper -> setLayout(emulator_controls_layout);

    // Set initial actual clock speed value
    clock_speed_value -> setText(clockSpeedDoubleToString(emulator -> clock_speed));

    // Set the actual clock speed value to auto update
    QTimer *clock_speed_refresh_timer = new QTimer(this);
    connect(clock_speed_refresh_timer, &QTimer::timeout, this, &MainWindow::updateRealClockRate);
    clock_speed_refresh_timer -> start(kClockSpeedRefreshMillis);

    // Bind the emulator controls
    connect(clock_speed_value, &QLineEdit::editingFinished, this, &MainWindow::updateClockRate);
    connect(step_button, &QPushButton::clicked, this, &MainWindow::emulatorStep);
    connect(run_button, &QPushButton::clicked, emulator, &Emulator::run);
    connect(interrupt_button, &QPushButton::clicked, this, &MainWindow::interruptEmulator);
}

double MainWindow::parseClockSpeedString(std::string clock_speed_string){
    // The offset of the unit in the string
    std::string::size_type unit_offset;
    // Strip any spaces
    remove(clock_speed_string.begin(), clock_speed_string.end(), ' ');
    // Grab the number
    double coefficient = std::stod(clock_speed_string, &unit_offset);
    // Grab the unit
    std::string unit = clock_speed_string.substr(unit_offset, clock_speed_string.length() - unit_offset - 1);
    // Interpret the unit
    if(unit == "Hz" || unit == ""){ // Base
        return coefficient;
    }
    if(unit.length() != 3 && unit.length() != 4) return -1; // Invalid format
    // Grab prefix
    char prefix = unit[0];
    // Scale number accordingly and return
    switch(prefix){
    case 'd':
        if(unit[2] == 'a') return 10 * coefficient;
        else return -1;
    case 'h':
        return 100 * coefficient;
    case 'k':
        return 1e3 * coefficient;
    case 'M':
        return 1e6 * coefficient;
    case 'G':
        return 1e9 * coefficient;
    default:
        return -1;
    }
}

void MainWindow::loadFile(std::string new_file_path){
    // Open and read the file into contents
    std::string contents;
    std::string line;

    std::ifstream new_file;
    new_file.open(new_file_path, ios::in);

    while(getline(new_file, line)){
        contents += line + "\n";
    }

    // Create a new LoadedFile object to be inserted into loadedFiles
    LoadedFile new_file_obj;

    // Break down the path to the file name and directory
    auto new_file_path_obj = std::filesystem::path(new_file_path);
    std::string new_file_name = new_file_path_obj.filename().u8string();
    std::string new_file_directory = new_file_path_obj.parent_path().u8string();

    // Populate the LoadedFile object and insert it into the loadedFiles vector
    new_file_obj.file_name = QString::fromStdString(new_file_name);
    new_file_obj.full_path = new_file_directory + SLASH + new_file_name;
    new_file_obj.contents = QString::fromStdString(contents);
    new_file_obj.saved_since_last_edit = false;
    loaded_files -> push_back(new_file_obj);

    // Add the file as an option to fileDropdown
    file_dropdown -> addItem(QString::fromStdString(new_file_path));
    file_dropdown -> setCurrentIndex(file_dropdown -> count() - 1);

    // Update the editor
    editor -> setPlainText(QString::fromUtf8(contents.data(), contents.length()));
    editor -> setDocumentTitle(QString::fromStdString(new_file_path));

    // Update the editor title
    editor_title -> setText(new_file_obj.file_name);
}

QString MainWindow::clockSpeedDoubleToString(double clock_speed){
    // If clock speed is 0, return "Stopped" instead
    if(clock_speed == 0) return "Stopped";
    // Get the order of magnitude and add the unit with the according SI prefix
    int order_of_magnitude = log10(clock_speed);
    switch (order_of_magnitude){
    case 0:
    case 1:
    case 2:
        return QString::number(clock_speed) + " Hz";
    case 3:
    case 4:
    case 5:
        return QString::number(clock_speed/1e3) + " kHz";
    default:
        return QString::number(clock_speed/1e6) + " MHz";
    }
}

void MainWindow::updateRealClockRate(){
    // Get the real clock speed, format it, then set the label in the emulator controls
    real_clock_speed_value -> setText(clockSpeedDoubleToString(emulator -> real_clock_speed));
}

void MainWindow::updateClockRate(){
    // Parse the provided clock speed then if it is valid, set the emulator clock speed to it
    double clock_rate = parseClockSpeedString(clock_speed_value -> text().toStdString());
    if(clock_rate != -1){
        emulator -> clock_speed = clock_rate;
    }
    // Then update the display
    clock_speed_value -> setText(clockSpeedDoubleToString(emulator -> clock_speed));
}

MainWindow::MainWindow(std::string kWindowTitle, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , kWindowTitle(kWindowTitle)
{
    // Set up window
    this -> setWindowTitle(kWindowTitle.data());
    this -> setWindowIcon(QIcon(":/resources/img/icon.xpm"));
    this -> resize(1200,600);

    // Set up the base widgets
    setUpMemoryTable(memory_view);
    updateRegisterTable(register_table);
    QVBoxLayout *editor_container_layout;
    setUpEditor(editor_container, editor, editor_title);
    setUpBuildLog(build_log);
    setUpEmulatorControls(emulator_controls_wrapper);


    // Set up the dock widgets
    register_tab = new QDockWidget("Registers");
    memory_tab = new QDockWidget("Memory");
    emulator_controls_tab = new QDockWidget("Controls");
    build_log_tab = new QDockWidget("Build Log");

    emulator_controls_tab -> setWidget(emulator_controls_wrapper);
    memory_tab -> setWidget(memory_view);
    register_tab -> setWidget(register_table);
    build_log_tab -> setWidget(build_log);

    // Dock widget title updates
    connect(memory_tab, &QDockWidget::topLevelChanged, this, &MainWindow::updateDockTitleMemory);
    connect(register_tab, &QDockWidget::topLevelChanged, this, &MainWindow::updateDockTitleRegisters);
    connect(emulator_controls_tab, &QDockWidget::topLevelChanged, this, &MainWindow::updateDockTitleControls);
    connect(build_log_tab, &QDockWidget::topLevelChanged, this, &MainWindow::updateDockTitleBuildLog);

    // Set up menus

    new_action = new QAction("New File");
    open_action = new QAction("Open");
    save_action = new QAction("Save");
    save_as_action = new QAction("Save As");
    close_action = new QAction("Close");

    build_action = new QAction(tr("&Build"));

    new_action -> setShortcut(Qt::CTRL | Qt::Key_N);
    open_action -> setShortcut(Qt::CTRL | Qt::Key_O);
    save_action -> setShortcut(Qt::CTRL | Qt::Key_S);
    save_as_action -> setShortcut(Qt::CTRL | Qt::SHIFT | Qt::Key_S);
    close_action -> setShortcut(Qt::CTRL | Qt::Key_W);
    build_action -> setShortcut(Qt::Key_F5);

    file_menu = menuBar() -> addMenu(tr("&File"));
    build_menu = menuBar() -> addMenu(tr("&Build"));

    file_menu->addAction(new_action);
    file_menu->addAction(open_action);
    file_menu->addAction(save_action);
    file_menu->addAction(save_as_action);
    file_menu->addAction(close_action);

    build_menu -> addAction(build_action);

    connect(new_action, &QAction::triggered, this, &MainWindow::handleMenuNew);
    connect(open_action, &QAction::triggered, this, &MainWindow::handleMenuOpen);
    connect(save_as_action, &QAction::triggered, this, &MainWindow::handleMenuSaveAs);
    connect(save_action, &QAction::triggered, this, &MainWindow::handleMenuSave);
    connect(close_action, &QAction::triggered, this, &MainWindow::handleMenuClose);

    connect(build_action, &QAction::triggered, this, &MainWindow::compileAndLoad);

    // Toolbar
    tool_bar = new QToolBar();
    file_dropdown =  new QComboBox();
    file_dropdown -> setSizeAdjustPolicy(QComboBox::AdjustToContents);
    file_dropdown_action = tool_bar -> addWidget(file_dropdown);

    loaded_files = new std::vector<LoadedFile>;

    connect(file_dropdown, &QComboBox::currentIndexChanged, this, &MainWindow::updateOpenFile);

    // Add everything to the window
    this -> setCentralWidget(editor_container);
    this -> addDockWidget(Qt::LeftDockWidgetArea, memory_tab);
    this -> addDockWidget(Qt::RightDockWidgetArea, register_tab);
    this -> addDockWidget(Qt::RightDockWidgetArea, emulator_controls_tab);
    this -> addDockWidget(Qt::BottomDockWidgetArea, build_log_tab);
    this -> addToolBar(tool_bar);

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

    delete run_button;
    delete interrupt_button;
    delete clock_speed_label;
    delete clock_speed_value;
    delete real_clock_speed_label;
    delete real_clock_speed_value;
    delete emulator_controls_wrapper;

    delete editor;
    delete editor_container;
    delete editor_title;

    delete build_log;

    delete build_button;

    delete memory_tab;
    delete register_tab;
    delete emulator_controls_tab;
    delete build_log_tab;

    delete file_menu;
    delete build_menu;

    delete save_action;
    delete save_as_action;
    delete new_action;
    delete open_action;
    delete close_action;
    delete build_action;

    delete tool_bar;
    delete file_dropdown;
    delete file_dropdown_action;

    delete loaded_files;

    delete open_dialog;

    delete syntax_highlighter;

    delete ui;
}

