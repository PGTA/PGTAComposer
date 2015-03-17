
#pragma once

#include <QMainWindow>
#include <QModelIndex>
#include <QTreeView>
#include <QDockWidget>
#include <thread>
#include <atomic>
#include <memory>

class PGTATrackTreeModel;
class PGTATrackView;
class QFileSystemModel;
class QDataWidgetMapper;
class PGTATreeView;
class PGTADockable;
class PGTAPropertiesView;

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    enum StatusBarState
    {
        StatusBarState_READY = 0,
        StatusBarState_OK,
        StatusBarState_ERROR,
    };

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
     void on_actionOpen_triggered();
     void on_actionSave_triggered();

     void insertSample();
     void removeTrackItem();
     void insertGroup();
     void onCustomContextMenu(const QPoint &point);
     void viewFullModel();
private slots: // playback
     void playTrack();
     void pauseTrack();
     void stopTrack();
    
private:
    Ui::MainWindow *ui;
    void updateStatusBar(QString message, StatusBarState state);

private: // dockables
    PGTADockable *m_trackDock;
    PGTATrackView *m_trackView;
    PGTADockable *m_propertiesDock;
    PGTAPropertiesView *m_propertiesView;

private: // models
    PGTATrackTreeModel *m_trackTreeModel;
    QFileSystemModel *m_fileSystemModel;
    QDataWidgetMapper *m_dataWidgetMapper;
    PGTATreeView *m_trackFullView;

private: // playback
    std::thread m_trackPlaybackThread;
    std::atomic<int> m_trackPlaybackControl;
    std::string m_playbackMessage;
};
