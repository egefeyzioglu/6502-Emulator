#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QFileDialog>
#include <QToolBar>
#include <QComboBox>

#include "loadedfile.h"
#include "syntaxhighlighter.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    /**
     * Updates the QDockWidget title depending on whether it is docked or not
     */
    void updateDockTitle(QDockWidget *dockWidget, bool isFloating);
    void updateDockTitleMemory(bool isFloating);
    void updateDockTitleRegisters(bool isFloating);
    void updateDockTitleControls(bool isFloating);

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
     * Updates the memory view QTableWidget object, reads from emulator using getMemoryValue
     *
     * TODO: Don't destroy the QTableWidget every time
     *
     * @param memory_table
     */
    void updateMemoryTable(QTableWidget *&memory_table);

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
    void setUpEditor(QTextEdit *&editor);

    /**
     * The main window title
     */
    const std::string kWindowTitle = "6502 Emulator";

private:
    Ui::MainWindow *ui;


    QTableWidget *memory_table = nullptr;
    QTableWidget *register_table = nullptr;
    QPushButton *step_button = nullptr;
    QTextEdit *editor = nullptr;

    QDockWidget *register_tab = nullptr;
    QDockWidget *memory_tab = nullptr;
    QDockWidget *emulator_controls_tab = nullptr;

    QMenu *fileMenu = nullptr;

    QAction *saveAction = nullptr;
    QAction *saveAsAction = nullptr;
    QAction *newAction = nullptr;
    QAction *openAction = nullptr;
    QAction *closeAction = nullptr;

    QToolBar *toolBar = nullptr;
    QComboBox *fileDropdown = nullptr;
    QAction *fileDropdownAction = nullptr;

    std::vector<LoadedFile> *loadedFiles = nullptr;

    QFileDialog *openDialog = nullptr;

    SyntaxHighlighter *syntaxHighlighter = nullptr;
};
#endif // MAINWINDOW_H
