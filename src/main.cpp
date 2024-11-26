#include <QApplication>
#include "MainWindow.h"

#ifdef Q_OS_WIN
#include <windows.h>
#endif

int main(int argc, char *argv[]) {
    #ifdef Q_OS_WIN
    // Hide console window
    ShowWindow(GetConsoleWindow(), SW_HIDE);
    #endif
    
    QApplication app(argc, argv);
    
    MainWindow mainWindow;
    mainWindow.show();
    
    return app.exec();
}



