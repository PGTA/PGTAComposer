#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Track.pb.h"
#include <QMainWindow>
#include <QModelIndex>
#include <vector>

namespace Ui {
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

    void on_actionNew_triggered();

    void on_actionSave_triggered();

    void on_listView_clicked(const QModelIndex &index);

    void on_save_sample_button_clicked();

    void on_add_sample_button_clicked();

    void on_remove_sample_button_clicked();

private:
    Ui::MainWindow *ui;
    PGTA::Track oldTrack;
    PGTA::Track newTrack;
    std::vector<PGTA::Track_Sample *> trackSamples;
    std::vector<PGTA::Track_Group *> trackGroups;
    int selectedSampleIndex;
    std::string fileName;
    void refreshListView();
    void refreshGroupsView();
    void saveTrackFile();
    void listViewClickHandler(const QModelIndex &index);
    QString getFileName(const std::string fileName);
    uint32_t probabilityToPercent(const uint32_t probability);
    uint32_t percentToProbability(const uint32_t percent);
};

#endif // MAINWINDOW_H
