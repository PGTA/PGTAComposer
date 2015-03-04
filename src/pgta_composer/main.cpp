#include <QtWidgets/QApplication>
#include <QDesktopWidget>
#include <QFile>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication mainApp(argc, argv);
    MainWindow mainWindow;

    // set stylesheet for application
    QFile stylesFile(":/styles/main.qss");
    stylesFile.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(stylesFile.readAll());
    mainApp.setStyleSheet(styleSheet);

    mainWindow.move(QApplication::desktop()->screen()->rect().center() - mainWindow.rect().center());
    mainWindow.show();
    return mainApp.exec();
}
