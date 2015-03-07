
#pragma once

#include <QMainWindow>
#include <QModelIndex>

class TrackTreeModel;
class QFileSystemModel;
class QDataWidgetMapper;

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
     void on_actionOpen_triggered();
     void on_actionSave_triggered();
     void treeViewRowColChange(const QModelIndex &index);
     void insertSample();
     void removeTrackItem();
     void clearSampleProperties();
     void insertGroup();
     void onCustomContextMenu(const QPoint &point);
     void toggleRightPanel();
     void toggleLeftPanel();
    
private:
    Ui::MainWindow *ui;

private:
    EngineTrack *m_engineTrack;
    TrackTreeModel *m_trackTreeModel;
    QFileSystemModel *m_fileSystemModel;
    QDataWidgetMapper *m_dataWidgetMapper;
};
