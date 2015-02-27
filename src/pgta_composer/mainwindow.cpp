#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStandardItem>
#include <QFileDialog>
#include <QStandardItemModel>
#include <sstream>      // std::ostringstream
#include <string>
#include <fstream>
#include <iostream>
#include <limits>
#include <stdint.h>
#include <memory>

//#include <QMediaPlayer>
#include <QFileSystemModel>
#include <QtCore>
#include <cmath>
#include "enginetrack.h"
#include "tracktablemodel.h"
#include "TrackTreeModel.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    m_engineTrack = new EngineTrack(this);
    m_engineTrack->init(QString::fromStdString("../../tracks/test1.track"));
    m_trackTableModel = new TrackTableModel(this);
    m_trackTableModel->setInput(m_engineTrack);
    
    QTreeView *test = new QTreeView(parent);
    test->setGeometry(0,0,200,200);

    TrackTreeModel *bla = new TrackTreeModel(this);

    test->setModel(bla);
    test->show();
    
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

/*uint32_t MainWindow::probabilityToPercent(const uint32_t probability)
{
    return ceil((static_cast<double>(probability) / UINT_MAX) * 100);
}

uint32_t MainWindow::percentToProbability(const uint32_t percent)
{
    return (static_cast<double>(percent) / 100) * UINT_MAX;
}


QString MainWindow::getFileName(const std::string fileName)
{
   const std::size_t slashIndex = fileName.find_last_of("\\/");
   return QString::fromStdString(fileName.substr(slashIndex+1));
}

int MainWindow::nameToSampleID(const std::string fileName)
{
    int numSamples = trackSamples.size();
    for (int i = 0; i < numSamples; ++i)
    {
        const PGTA::Track_Sample *sample = trackSamples.at(i);
        if (getFileName(sample->filepath()).toStdString() == fileName)
        {
            return i;
        }
    }
    return -1;
}

void MainWindow::setGroupsFromView()
{
    oldTrack = newTrack;
    newTrack.Clear();
    trackSamples.clear();
    trackGroups.clear();

    for (int i = 0; i< oldTrack.samples().size(); ++i)
    {
        // PROTOBUF STUFF
        const PGTA::Track_Sample oldSample = oldTrack.samples().Get(i);
        trackSamples.push_back(newTrack.add_samples());
        trackSamples[i]->set_filepath(oldSample.filepath());
        trackSamples[i]->set_starttime(oldSample.starttime());
        trackSamples[i]->set_frequency(oldSample.frequency());
        trackSamples[i]->set_probability(oldSample.probability());
        trackSamples[i]->set_volumemultiplier(oldSample.volumemultiplier());
    }

    QAbstractItemModel *model = ui->treeView->model();
    int numGroups = model->rowCount();
    for (int i = 0; i < numGroups; ++i)
    {
        // PROTOBUF STUFF
        trackGroups.push_back(newTrack.add_groups());
        // READING STUFF FROM VIEW
        QModelIndex parentIndex = model->index(i,0);
        int numSamples = model->rowCount(parentIndex);
        // CREATE NEW GROUP
        for (int j = 0; j < numSamples; ++j)
        {
            QModelIndex sampleIndex = model->index(j,0,parentIndex);
            QString fileName = sampleIndex.data().toString();
            int sampleID = nameToSampleID(fileName.toStdString());
            if (sampleID == -1)
            {
                continue;
            }
            // ADD SAMPLE TO GROUP
            trackGroups[i]->add_sampleindex(sampleID);
        }
    }
    refreshGroupsView();
}

void MainWindow::saveTrackFile()
{
    // Update Groups based on current Qt model
    setGroupsFromView();

    if (fileName.length() == 0)
    {
        QString file = QFileDialog::getSaveFileName(this,tr("Save Track File"),"",tr("Track files (*.track)"));
        if (file.isEmpty()) {
            ui->statusbar->showMessage("Error saving track file!");
            return;
        }
        fileName = file.toStdString();
    }

    std::fstream output(fileName.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
    if (!newTrack.SerializePartialToOstream(&output))
    {
        ui->statusbar->showMessage("Error saving track file!");
        return;
    }

    //delete all global object allocated by libprotobuf
    google::protobuf::ShutdownProtobufLibrary();
    ui->tabWidget->setTabText(ui->tabWidget->currentIndex(),getFileName(fileName));
    ui->statusbar->showMessage("Track file saved!");
}

void MainWindow::on_actionNew_triggered()
{
    newTrack.Clear();
    oldTrack.Clear();
    trackSamples.clear();
    trackGroups.clear();
    fileName = "";
    clearSamplePropertiesFields();
    ui->tabWidget->setTabText(ui->tabWidget->currentIndex(),QString::fromStdString("untitled.track"));
    refreshListView();
    refreshGroupsView();
}


void MainWindow::on_add_group_clicked()
{
    std::ostringstream groupName;
    groupName << "Group " << ui->treeView->model()->rowCount()+1;
    QStandardItem *groupItem = new QStandardItem(QString::fromStdString(groupName.str()));
    QStandardItemModel *model = dynamic_cast<QStandardItemModel*>(ui->treeView->model());
    groupItem->setEditable(false);
    groupItem->setDropEnabled(true);
    model->invisibleRootItem()->appendRow(groupItem);
}

void MainWindow::on_remove_group_selection_clicked()
{
    if (selectedGroupElementIndex.isValid())
    {
        ui->treeView->model()->removeRow(selectedGroupElementIndex.row(),selectedGroupElementIndex.parent());
    }
    selectedGroupElementIndex = QModelIndex(); // reset to invalid
}


void MainWindow::on_actionSave_triggered()
{
    saveTrackFile();
}

void MainWindow::on_actionOpen_triggered()
{
    QStringList fileList = QFileDialog::getOpenFileNames(this, tr("Open Track File"),"",tr("Track files (*.track)"));
    if (fileList.size() <= 0) {
        return;
    }

    newTrack.Clear();
    oldTrack.Clear();
    trackSamples.clear();
    trackGroups.clear();

    clearSamplePropertiesFields();

    fileName = fileList.first().toStdString().c_str();

    std::fstream input(fileName.c_str(), std::ios::in | std::ios::binary);
    if (!input)
    {
        ui->statusbar->showMessage("Invalid track file!");
        return;
    }

    bool parsed = oldTrack.ParseFromIstream(&input);
    input.close();

    if (!parsed)
    {
        ui->statusbar->showMessage("Failed to parse track!");
        return;
    }

    for (int i = 0; i< oldTrack.samples().size(); i++)
    {
        const PGTA::Track_Sample oldSample = oldTrack.samples().Get(i);
        trackSamples.push_back(newTrack.add_samples());
        trackSamples[i]->set_filepath(oldSample.filepath());
        trackSamples[i]->set_starttime(oldSample.starttime());
        trackSamples[i]->set_frequency(oldSample.frequency());
        trackSamples[i]->set_probability(oldSample.probability());
        trackSamples[i]->set_volumemultiplier(oldSample.volumemultiplier());
    }

    for (int i = 0; i< oldTrack.groups().size(); i++)
    {
        const PGTA::Track_Group oldGroup = oldTrack.groups().Get(i);
        trackGroups.push_back(newTrack.add_groups());

        for (int j = 0; j < oldGroup.sampleindex().size(); j++) {
            trackGroups[i]->add_sampleindex(oldGroup.sampleindex().Get(j));
        }
    }

    ui->tabWidget->setTabText(ui->tabWidget->currentIndex(),getFileName(fileName));
    refreshListView();
    refreshGroupsView();
}

void MainWindow::refreshListView(){
    QStandardItemModel* listModel = new QStandardItemModel();
    for (auto *sample : trackSamples)
    {
        QStandardItem *items = new QStandardItem(getFileName(sample->filepath()));
        items->setEditable(false);
        listModel->appendRow(items);
    }
    ui->listView->setModel(listModel);
}

void MainWindow::refreshGroupsView()
{
    // read what is on the screen
    QStandardItemModel* groupsModel = new QStandardItemModel();
    groupsModel->invisibleRootItem()->setDropEnabled(false);
    int numGroups = trackGroups.size();
    for (auto groupID = 0; groupID < numGroups; ++groupID)
    {
        std::ostringstream groupName;
        groupName << "Group " << groupID+1;
        QStandardItem *groupItem = new QStandardItem(QString::fromStdString(groupName.str()));

        for(auto sampleID : trackGroups.at(groupID)->sampleindex())
        {
            QString sampleName = getFileName(trackSamples.at(sampleID)->filepath());
            QStandardItem *sampleItem = new QStandardItem(sampleName);
            sampleItem->setEditable(false);
            sampleItem->setDropEnabled(false);
            groupItem->appendRow(sampleItem);
        }
        groupItem->setEditable(false);
        groupItem->setDropEnabled(true);
        groupsModel->invisibleRootItem()->appendRow(groupItem);
    }
    ui->treeView->setModel(groupsModel);
}

void MainWindow::on_listView_clicked(const QModelIndex &index)
{
    listViewClickHandler(index);
}

void MainWindow::on_treeView_clicked(const QModelIndex &index)
{
    selectedGroupElementIndex = index;
    return;
}

void MainWindow::clearSamplePropertiesFields()
{
    QString qs("");
    ui->sample_path_edit->setText(qs);
    ui->sample_start_time->setText(qs);
    ui->sample_frequency_edit->setText(qs);
    ui->sample_prob_edit->setText(qs);
    ui->sample_vol_edit->setText(qs);
}

void MainWindow::listViewClickHandler(const QModelIndex &index) {
    selectedSampleIndex = index.row();
    PGTA::Track_Sample* selectedSample = trackSamples[selectedSampleIndex];
    QString qs(selectedSample->filepath().c_str());
    ui->sample_path_edit->setText(qs);
    qs = QString::number(selectedSample->starttime());
    ui->sample_start_time->setText(qs);
    qs = QString::number(selectedSample->frequency());
    ui->sample_frequency_edit->setText(qs);
    qs = QString::number(probabilityToPercent(selectedSample->probability()));
    ui->sample_prob_edit->setText(qs);
    qs = QString::number(selectedSample->volumemultiplier());
    ui->sample_vol_edit->setText(qs);
}

void MainWindow::on_save_sample_button_clicked()
{
    if (selectedSampleIndex == -1)
    {
        return;
    }

    PGTA::Track_Sample* selectedSample = trackSamples[selectedSampleIndex];

    int new_frequency;
    uint32_t new_prob;
    unsigned long start_time;
    float new_vol;
    QString new_path;

    try
    {
        new_path = ui->sample_path_edit->text();
        start_time = ui->sample_start_time->text().toULong();
        new_frequency = ui->sample_frequency_edit->text().toInt();
        new_prob = percentToProbability(ui->sample_prob_edit->text().toUInt());
        new_vol = ui->sample_vol_edit->text().toFloat();
    } catch (int e)
    {
        ui->statusbar->showMessage("Invalid Sample Parameters!");
        return;
    }

    selectedSample->set_filepath(new_path.toStdString());
    selectedSample->set_starttime(start_time);
    selectedSample->set_frequency(new_frequency);
    selectedSample->set_probability(new_prob);
    selectedSample->set_volumemultiplier(new_vol);

    refreshListView();

    ui->statusbar->showMessage("Sample saved!");

}

void MainWindow::on_add_sample_button_clicked()
{
    trackSamples.push_back(newTrack.add_samples());
    refreshListView();
    QModelIndex modelIndex = ui->listView->model()->index(trackSamples.size()-1,0);
    ui->listView->setCurrentIndex(modelIndex);
    listViewClickHandler(modelIndex);
}

void MainWindow::on_remove_sample_button_clicked()
{
    // Update Groups based on the current Qt model
    setGroupsFromView();

    QModelIndex currentIndex = ui->listView->currentIndex();

    if (currentIndex.row() < 0)
    {
        return;
    }

    oldTrack = newTrack;
    newTrack.Clear();
    trackSamples.clear();
    trackGroups.clear();

    for (int i = 0; i< oldTrack.samples().size(); ++i)
    {
        if (i == currentIndex.row()){
            continue;
        }
        int index = i;
        if (index > currentIndex.row())
        {
            --index;
        }
        const PGTA::Track_Sample oldSample = oldTrack.samples().Get(i);
        trackSamples.push_back(newTrack.add_samples());
        trackSamples[index]->set_filepath(oldSample.filepath());
        trackSamples[index]->set_starttime(oldSample.starttime());
        trackSamples[index]->set_frequency(oldSample.frequency());
        trackSamples[index]->set_probability(oldSample.probability());
        trackSamples[index]->set_volumemultiplier(oldSample.volumemultiplier());
    }

    for (int i = 0; i < oldTrack.groups().size(); i++)
    {
        const PGTA::Track_Group oldGroup = oldTrack.groups().Get(i);
        std::unique_ptr<PGTA::Track_Group> newGroup(new PGTA::Track::Group());

        for (int j = 0; j < oldGroup.sampleindex().size(); j++) {
            int index = oldGroup.sampleindex().Get(j);
            if (index == currentIndex.row())
            {
                continue;
            }
            if (index > currentIndex.row())
            {
                --index;
            }
            newGroup->add_sampleindex(index);
        }
        if (newGroup->sampleindex_size() > 0)
        {
            trackGroups.push_back(newGroup.get());
            newTrack.mutable_groups()->AddAllocated(newGroup.release());
        }
    }
    clearSamplePropertiesFields();
    refreshListView();
    refreshGroupsView();
}*/
