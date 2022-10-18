#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QFileDialog>
#include <QToolBar>
#include <QComboBox>
#include <QLabel>

#include "loadedfile.h"
#include "qplaintextedit.h"
#include "syntaxhighlighter.h"
#include "memorymodel.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(std::string kWindowTitle, QWidget *parent = nullptr);
    ~MainWindow();

    /**
     * Updates the QDockWidget title depending on whether it is docked or not
     */
    void updateDockTitle(QDockWidget *dock_widget, bool is_floating);
    // Slots for when the dock widgets start and stop floating
    void updateDockTitleMemory(bool is_floating);
    void updateDockTitleRegisters(bool is_floating);
    void updateDockTitleControls(bool is_floating);
    void updateDockTitleBuildLog(bool is_floating);

    /**
     * Compiles and loads the current file into memory at 0x5F00, sets the reset vector to that address, and resets the emulator
     *
     * TODO: Move the positioning in memory to an external script and call that
     * TODO: Allow custom memory maps
     */
    void compileAndLoad();

    /**
     * Handles the open action click
     */
    void handleMenuOpen();

    /**
     * Handles the save as action click
     */
    void handleMenuSaveAs();

    /**
     * Handles the save action click
     */
    void handleMenuSave();

    /**
     * Handles the new action click
     */
    void handleMenuNew();

    /**
     * Handles the close action click
     */
    void handleMenuClose();

    /**
     * Updates the current open file contents in openFileContents
     */
    void updateOpenFileContents();

    /**
     * Update the current file displayed by the editor
     */
    void updateOpenFile(int selected_file_index);

    /**
     * Step the emulator one instruction
     */
    void emulatorStep();

    /**
     * Sets up the memory view QTableView object
     *
     * TODO: Don't destroy the QTableWidget every time
     *
     * @param memory_table
     */
    void setUpMemoryTable(QTableView *&memory_view);

    /**
     * Updates the register view QTableWidget object, reads from emulator using getMemoryValue
     *
     * TODO: Don't destroy the QTableWidget every time
     *
     * @param register_table
     */
    void updateRegisterTable(QTableWidget *&register_table);

    /**
     *
     * Sets up the editor
     *
     * @param editor_container
     * @param editor
     * @param editor_title
     */
    void setUpEditor(QWidget *&editor_container, QTextEdit *&editor, QLabel *&editor_title);

    /**
     * Sets up the emulator controls
     *
     * @param emulator_controls_wrapper
     * @return
     */
    void setUpEmulatorControls(QWidget *&emulator_controls_wrapper);

    /**
     * Resets the emulator
     *
     */
    void resetEmulator();

    /**
     * Sets up the build log widget
     *
     * @param compiler_log
     */
    void setUpBuildLog(QPlainTextEdit *&compiler_log);

    /**
     * Adds new line or lines to the build log
     *
     * @param new_content
     */
    void addToBuildLog(QString new_content);

    /**
     * Load a new file at specified path
     *
     * @param new_file_path
     */
    void loadFile(std::string new_file_path);

    /**
     * Highlight register display to show that the register changed
     * @param reg the register
     */
    void handleRegistersChanged(std::vector<Emulator::Register> registers_to_update);

    /**
     * Updates the real clock rate display
     */
    void updateRealClockRate();

    /**
     * Formats the clock speed with the correct SI prefix and returns as QString
     * @param clock_speed
     * @return The formatted clock speed
     */
    QString clockSpeedDoubleToString(double clock_speed);

    /**
     * Updates the emulator clock rate from the input field
     */
    void updateClockRate();

    /**
     * Interrupt the emulator
     */
    void interruptEmulator();

    /**
     * The main window title
     */
    const std::string kWindowTitle;

    /**
     * The default size of the build log
     */
    const int kBuildLogDefaultSize = 130;

    /**
     * How often the real clock speed display is updated
     */
    const int kClockSpeedRefreshMillis = 500;

private:
    Ui::MainWindow *ui;

    // The memory display/model
    QTableView *memory_view = nullptr;
    MemoryModel *memory_model = nullptr;

    // The register table
    QTableWidget *register_table = nullptr;

    // The emulator control stuff and the wrapper
    QPushButton *step_button = nullptr;
    QPushButton *run_button = nullptr;
    QPushButton *interrupt_button = nullptr;
    QLabel *clock_speed_label = nullptr;
    QLineEdit *clock_speed_value = nullptr;
    QLabel *real_clock_speed_label = nullptr;
    QLabel *real_clock_speed_value = nullptr;
    QWidget *emulator_controls_wrapper = nullptr;


    // The editor, its title, and their container
    QTextEdit *editor = nullptr;
    QWidget *editor_container = nullptr;
    QLabel *editor_title = nullptr;

    // The build output area
    QPlainTextEdit *build_log = nullptr;

    // The build button, unused (TODO: Remove)
    QPushButton *build_button = nullptr;

    // The dock widgets
    QDockWidget *register_tab = nullptr;
    QDockWidget *memory_tab = nullptr;
    QDockWidget *emulator_controls_tab = nullptr;
    QDockWidget *build_log_tab = nullptr;

    // The menus
    QMenu *file_menu = nullptr;
    QMenu *build_menu = nullptr;

    // Actions in the menus (individual thingies you click on)
    QAction *save_action = nullptr;
    QAction *save_as_action = nullptr;
    QAction *new_action = nullptr;
    QAction *open_action = nullptr;
    QAction *close_action = nullptr;
    QAction *build_action = nullptr;

    // The toolbar and the stuff in the toolbar
    QToolBar *tool_bar = nullptr;
    QComboBox *file_dropdown = nullptr;
    QAction *file_dropdown_action = nullptr;

    /**
     * Vector of the `LoadedFile`s opened by the editor
     */
    std::vector<LoadedFile> *loaded_files = nullptr;

    /**
     * The open file dialog
     */
    QFileDialog *open_dialog = nullptr;

    /**
     * The syntax highlighter object
     */
    SyntaxHighlighter *syntax_highlighter = nullptr;

    /**
     * Converts the given string in expressing clock frequency into a double.
     * Is able to understand "Hz", "daHz", "kHz", "MHz", and just a number (interpreted as Hertz.)
     *
     * @param clockSpeedString
     * @return
     */
    double parseClockSpeedString(std::string clockSpeedString);
};
#endif // MAINWINDOW_H
