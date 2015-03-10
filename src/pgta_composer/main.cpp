#include <QtWidgets/QApplication>
#include <QDesktopWidget>
#include <QFile>
#include <QDir>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication mainApp(argc, argv);
    MainWindow mainWindow;

    QDir dir(QApplication::applicationDirPath());
    QApplication::addLibraryPath(dir.absolutePath());

    // set stylesheet for application
    QFile stylesFile(":/styles/main.qss");
    stylesFile.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(stylesFile.readAll());
    mainApp.setStyleSheet(styleSheet);

    QIcon icon(":icons/icon.icns");
    mainWindow.setWindowIcon(icon);
    //mainWindow.setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    mainWindow.move(QApplication::desktop()->screen()->rect().center() - mainWindow.rect().center());
    mainWindow.show();
    return mainApp.exec();
}
