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
    void updateDockTitle(QDockWidget *dockWidget, bool isFloating);
    void updateDockTitleMemory(bool isFloating);
    void updateDockTitleRegisters(bool isFloating);
    void updateDockTitleControls(bool isFloating);
    void updateDockTitleBuildLog(bool isFloating);

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
    void updateOpenFile(int selectedFileIndex);

    /**
     * Step the emulator one instruction
     */
    void emulatorStep();

    /**
     * Sets up the memory view QTableView object
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
     * Sets up the editor
     *
     * @param editor
     */
    void setUpEditor(QWidget *&editorContainer, QTextEdit *&editor, QLabel *&editorTitle);

    /**
     * Resets the emulator
     *
     */
    void resetEmulator();

    /**
     * Sets up the build log widget
     *
     * @param compilerLog
     */
    void setUpBuildLog(QPlainTextEdit *&compilerLog);

    /**
     * Adds new line or lines to the build log
     *
     * @param newContent
     */
    void addToBuildLog(QString newContent);

    /**
     * Load a new file at specified path
     *
     * @param newFilePath
     */
    void loadFile(std::string newFilePath);

    /**
     * Highlight register display to show that the register changed
     * @param reg the register
     */
    void handleRegistersChanged(std::vector<Emulator::Register> registers_to_update);

    /**
     * The main window title
     */
    const std::string kWindowTitle;

    /**
     * The default size of the build log
     */
    const int kBuildLogDefaultSize = 130;

private:
    Ui::MainWindow *ui;

    QTableView *memory_view;
    MemoryModel *memory_model;
    QTableWidget *register_table = nullptr;
    QPushButton *step_button = nullptr;

    QTextEdit *editor = nullptr;
    QWidget *editorContainer = nullptr;
    QLabel *editorTitle = nullptr;

    QPlainTextEdit *buildLog = nullptr;

    QPushButton *build_button = nullptr;

    QDockWidget *register_tab = nullptr;
    QDockWidget *memory_tab = nullptr;
    QDockWidget *emulator_controls_tab = nullptr;
    QDockWidget *build_log_tab = nullptr;

    QMenu *fileMenu = nullptr;
    QMenu *buildMenu = nullptr;

    QAction *saveAction = nullptr;
    QAction *saveAsAction = nullptr;
    QAction *newAction = nullptr;
    QAction *openAction = nullptr;
    QAction *closeAction = nullptr;
    QAction *buildAction = nullptr;

    QToolBar *toolBar = nullptr;
    QComboBox *fileDropdown = nullptr;
    QAction *fileDropdownAction = nullptr;

    std::vector<LoadedFile> *loadedFiles = nullptr;

    QFileDialog *openDialog = nullptr;

    SyntaxHighlighter *syntaxHighlighter = nullptr;
};
#endif // MAINWINDOW_H
