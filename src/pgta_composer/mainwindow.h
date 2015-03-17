
#pragma once

#include <QMainWindow>
#include <QModelIndex>
#include <QTreeView>
#include <QDockWidget>
#include <thread>
#include <atomic>
#include <memory>
#include <PGTATestCommon.h>

class PGTATrackTreeModel;
class PGTATrackView;
class QFileSystemModel;
class QDataWidgetMapper;
class PGTATreeView;
class PGTADockable;
class PGTAPropertiesView;
class QMediaPlayer;

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
     void viewFullModel();
     void slotLibraryMediaClicked(QModelIndex index);
     void slotNewTrack();

private slots: // playback
     void playTrack();
     void pauseTrack();
     void stopTrack();
     void slotUpdateVolumeMultiplier(int value);
    
private:
    Ui::MainWindow *ui;
    void updateStatusBar(QString message, StatusBarState state);
    void PlaySample(const QString &filePath);
    void OpenTrack(const std::string &filePath);
    bool SaveTrack();

private: // dockables
    PGTADockable *m_trackDock;
    PGTATrackView *m_trackView;
    PGTADockable *m_propertiesDock;
    PGTAPropertiesView *m_propertiesView;
    PGTADockable *m_libraryDock;
    QTreeView *m_libraryView;
private: // views
    PGTATreeView *m_trackFullView;

private: // models
    PGTATrackTreeModel *m_trackTreeModel;
    QFileSystemModel *m_libraryModel;
    QDataWidgetMapper *m_dataWidgetMapper;

private: // playback
    std::thread m_trackPlaybackThread;
    std::atomic<PlaybackControl> m_trackPlaybackControl;
    std::atomic<uint8_t> m_volumeMultiplier;
    std::string m_playbackMessage;
    QMediaPlayer *m_mediaPlayer;
};
