#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStandardItem>
#include <QFileDialog>
#include <QStandardItemModel>
#include <QTreeView>
#include <sstream>
#include <string>
#include <fstream>
#include <iostream>
#include <limits>
#include <stdint.h>
#include <memory>

//#include <QMediaPlayer>
#include <QFileSystemModel>
#include <QDataWidgetMapper>
#include <QtCore>
#include <cmath>
#include "enginetrack.h"
#include "tracktablemodel.h"
#include "TrackTreeModel.h"
#include "FlatbufferTrackLoader.h"
#include "FileUtils.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    m_fileSystemModel= new QFileSystemModel();
    m_fileSystemModel->setRootPath(QDir::currentPath());

    m_trackTreeModel = new TrackTreeModel(this);

    ui->setupUi(this);
    // remove title bar from all dock widgets
    ui->TopPanel->setTitleBarWidget(new QWidget());
    ui->LeftPanel->setTitleBarWidget(new QWidget());
    ui->RightPanel->setTitleBarWidget(new QWidget());

    ui->TrackTreeView->setStyleSheet("QTreeView {background: #252526; color: #FFFFFF;}");

    // set modles
    ui->FileSystemView->setModel(m_fileSystemModel);
    ui->TrackTreeView->setModel(m_trackTreeModel);

    // setup data widget mapper
    m_dataWidgetMapper = new QDataWidgetMapper(this);
    m_dataWidgetMapper->setModel(m_trackTreeModel);
    m_dataWidgetMapper->addMapping(ui->EditName, TrackTreeModel::SampleColumn_Name);
    m_dataWidgetMapper->addMapping(ui->EditDefaultFile, TrackTreeModel::SampleColumn_DefaultFile);
    m_dataWidgetMapper->addMapping(ui->EditStartTime, TrackTreeModel::SampleColumn_StartTime);
    m_dataWidgetMapper->addMapping(ui->EditFrequency, TrackTreeModel::SampleColumn_Frequency);
    m_dataWidgetMapper->addMapping(ui->EditProbability, TrackTreeModel::SampleColumn_Probability);
    m_dataWidgetMapper->addMapping(ui->EditVolumeMultiplier, TrackTreeModel::SampleColumn_VolumeMultiplier);
    m_dataWidgetMapper->addMapping(ui->EditGroup, TrackTreeModel::SampleColumn_GroupUUID);
    connect(ui->TrackTreeView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
        this, SLOT(treeViewRowColChange(QModelIndex)));

    connect(ui->TrackTreeView, SIGNAL(customContextMenuRequested(const QPoint &)), this,
            SLOT(onCustomContextMenu(const QPoint &)));

    connect(ui->insertSampleAction, SIGNAL(triggered()), this, SLOT(insertSample()));
    connect(ui->insertGroupAction, SIGNAL(triggered()), this, SLOT(insertGroup()));
    connect(ui->removeTrackItemAction, SIGNAL(triggered()), this, SLOT(removeTrackItem()));


}

MainWindow::~MainWindow()
{
    delete m_dataWidgetMapper;
    delete m_trackTreeModel;
    delete m_fileSystemModel;
    delete ui;
}

void MainWindow::onCustomContextMenu(const QPoint &point)
{
    QModelIndex index = ui->TrackTreeView->indexAt(point);
    QPoint globalPos = ui->TrackTreeView->mapToGlobal(point);
    TrackTreeModel *model = static_cast<TrackTreeModel*>(ui->TrackTreeView->model());
    QMenu myMenu;

    QAction *insertSampleAction = myMenu.addAction("Insert Sample");
    connect(insertSampleAction, SIGNAL(triggered()), this, SLOT(insertSample()));

    if (index.isValid())
    {
        QString removeText = "Remove Sample";
        if (model->isGroup(index))
        {
            removeText = "Remove Group";
        }
        QAction *removeTrackItemAction = myMenu.addAction(removeText);
        connect(removeTrackItemAction, SIGNAL(triggered()), this, SLOT(removeTrackItem()));
    }
    else
    {
        QAction *insertGroupAction = myMenu.addAction("Insert Group");
        connect(insertGroupAction, SIGNAL(triggered()), this, SLOT(insertGroup()));
    }
    myMenu.exec(globalPos);
}

void MainWindow::treeViewRowColChange(const QModelIndex &index)
{
    m_dataWidgetMapper->setRootIndex(index.parent());
    m_dataWidgetMapper->setCurrentModelIndex(index);
}

void MainWindow::insertGroup()
{
    TrackTreeModel *model = static_cast<TrackTreeModel*>(ui->TrackTreeView->model());
    QModelIndex rootIndex = ui->TrackTreeView->rootIndex();
    QModelIndex index = ui->TrackTreeView->selectionModel()->currentIndex();
    if (index.parent() != rootIndex)
    {
        index = rootIndex;
    }

    if (!model->insertRow(index.row()+1, index.parent()))
    {
        return;
    }

    for (int column = 0; column < model->columnCount(index.parent()); ++column)
    {
        QModelIndex child = model->index(index.row()+1, column, index.parent());
        //TODO: Add UUID generation
        model->setData(child, QVariant("[Group No data]"), Qt::EditRole);
        model->setIsGroup(child);
    }
}

void MainWindow::removeTrackItem()
{
    QModelIndex index = ui->TrackTreeView->selectionModel()->currentIndex();

    if (!index.isValid())
    {
        return;
    }

    TrackTreeModel *model = static_cast<TrackTreeModel*>(ui->TrackTreeView->model());
    if(model->removeRow(index.row(), index.parent()))
    {
        //TODO: remove UUID from aux data structure
        ui->TrackTreeView->selectionModel()->clear();
        clearSampleProperties();
    }
}

void MainWindow::clearSampleProperties()
{
    ui->EditName->clear();
    ui->EditDefaultFile->clear();
    ui->EditStartTime->clear();
    ui->EditFrequency->clear();
    ui->EditProbability->clear();
    ui->EditVolumeMultiplier->clear();
    ui->EditGroup->clear();
}

void MainWindow::insertSample()
{
    QModelIndex selectedIndex = ui->TrackTreeView->selectionModel()->currentIndex();
    TrackTreeModel *model = static_cast<TrackTreeModel*>(ui->TrackTreeView->model());

    int position = 0;
    // set column to column 0 otherwise inserting child doesn't work
    QModelIndex index = model->index(selectedIndex.row(), 0, selectedIndex.parent());

    // check if selection is on first level
    if (!model->isGroup(index))
    {
        position = index.row() + 1; // index of current selection + 1
        index = index.parent();
    }

    if (!model->insertRow(position, index))
    {
        return;
    }

    for (int column = 0; column < model->columnCount(index); ++column)
    {
        QModelIndex child = model->index(position, column, index);
        //TODO: set UUID correclty
        model->setData(child, QVariant("[No data]"), Qt::EditRole);
        if (!model->headerData(column, Qt::Horizontal).isValid())
        {
            model->setHeaderData(column, Qt::Horizontal, QVariant("[No header]"), Qt::EditRole);
        }
    }
    ui->TrackTreeView->selectionModel()->setCurrentIndex(model->index(position, 0, index),
                                            QItemSelectionModel::ClearAndSelect);
}

void MainWindow::on_actionSave_triggered()
{
    QModelIndex index = ui->TrackTreeView->selectionModel()->currentIndex();
     m_dataWidgetMapper->toNext();
}

void MainWindow::on_actionOpen_triggered()
{
    if (m_trackTreeModel)
    {
        delete m_trackTreeModel;
    }

    if (m_dataWidgetMapper)
    {
        delete m_dataWidgetMapper;
    }

    m_trackTreeModel = new TrackTreeModel(this);
    m_dataWidgetMapper = new QDataWidgetMapper(this);
    QStringList fileList = QFileDialog::getOpenFileNames(this, tr("Open Track File"),"",tr("Track files (*.track)"));
    if (fileList.isEmpty())
    {
        ui->statusBar->showMessage("Error opening file.");
        return;
    }

    std::string fileName = fileList.first().toStdString();
    std::string buffer;
    if (!FileUtils::ReadBinaryFileToString(fileName, buffer))
    {
        qDebug("Error reading from file.");
        ui->statusBar->showMessage("Error reading from file.");
        ui->statusBar->setStyleSheet("QStatusBar{background: #C85217; color: #FFFFFF;}");
        return;
    }

    // initialize track model
    if(!FlatbufferTrackLoader::LoadTrack(buffer.c_str(), buffer.length(), m_trackTreeModel))
    {
        ui->statusBar->showMessage("Error reading from file.");
        ui->statusBar->setStyleSheet("QStatusBar{background: #C85217; color: #FFFFFF;}");
    }
    else
    {
        ui->statusBar->showMessage("File opened successfully.");
        ui->statusBar->setStyleSheet("QStatusBar{background: #137CCA; color: #FFFFFF;}");
    }

    ui->TrackTreeView->setModel(m_trackTreeModel);
    m_dataWidgetMapper->setModel(m_trackTreeModel);
    m_dataWidgetMapper->addMapping(ui->EditName, TrackTreeModel::SampleColumn_Name);
    m_dataWidgetMapper->addMapping(ui->EditDefaultFile, TrackTreeModel::SampleColumn_DefaultFile);
    m_dataWidgetMapper->addMapping(ui->EditStartTime, TrackTreeModel::SampleColumn_StartTime);
    m_dataWidgetMapper->addMapping(ui->EditFrequency, TrackTreeModel::SampleColumn_Frequency);
    m_dataWidgetMapper->addMapping(ui->EditProbability, TrackTreeModel::SampleColumn_Probability);
    m_dataWidgetMapper->addMapping(ui->EditVolumeMultiplier, TrackTreeModel::SampleColumn_VolumeMultiplier);
    m_dataWidgetMapper->addMapping(ui->EditGroup, TrackTreeModel::SampleColumn_GroupUUID);
    connect(ui->TrackTreeView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
        this, SLOT(treeViewRowColChange(QModelIndex)));
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
