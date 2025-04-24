/**
 * @file main.cpp
 * @brief Main entry point for the application
 */
#include <QApplication>
#include "mainwindow.h"

/**
 * @brief Main entry point
 * @param argc Argument count
 * @param argv Argument vector
 * @return Application exit code
 */
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Set application information
    QApplication::setApplicationName("Air Quality Monitor");
    QApplication::setApplicationVersion("1.0");
    QApplication::setOrganizationName("JPO Project");

    // Create and show the main window
    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}
