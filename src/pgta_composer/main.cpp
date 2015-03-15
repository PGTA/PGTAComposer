#include <QtWidgets/QApplication>
#include <QDesktopWidget>
#include <QFile>
#include <QDir>
#include <QTime>
#include <string>
#include "mainwindow.h"

// use qDebug(), qWarning(), qCritical(), qFatal() to log
void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    std::string currentTime = QTime::currentTime().toString("hh:mm:ss.zzz").toStdString();
    QByteArray localMsg = msg.toLocal8Bit();
    switch (type) {
    case QtDebugMsg:
        fprintf(stderr, "%s [DEBUG]: %s (%s:%u)\n", currentTime.c_str(), localMsg.constData(),
                context.file, context.line);
        break;
    case QtWarningMsg:
        fprintf(stderr, "%s [WARNING]: %s (%s:%u)\n", currentTime.c_str(), localMsg.constData(),
                context.file, context.line);
        break;
    case QtCriticalMsg:
        fprintf(stderr, "%s [CRITICAL]: %s (%s:%u)\n", currentTime.c_str(), localMsg.constData(),
                context.file, context.line);
        break;
    case QtFatalMsg:
        fprintf(stderr, "%s [FATAL]: %s (%s:%u)\n", currentTime.c_str(), localMsg.constData(),
                context.file, context.line);
        abort();
    }
}

int main(int argc, char *argv[])
{
    qInstallMessageHandler(myMessageOutput);
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
