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
#include <PGTATestCommon.h>
#include <QFileSystemModel>
#include <QDataWidgetMapper>
#include <QMessageBox>
#include <QToolTip>
#include <QUuid>
#include <QtCore>
#include <atomic>
#include <cmath>
#include <thread>
#include <functional>
#include "PGTATrackTreeModel.h"
#include "FlatbufferTrackLoader.h"
#include "FlatbufferTrackWriter.h"
#include "FileUtils.h"
#include "PGTATreeView.h"
#include "PGTATrackView.h"
#include "PGTADockable.h"
#include "PGTAPropertiesView.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_trackDock(nullptr),
    m_trackView(nullptr),
    m_propertiesDock(nullptr),
    m_propertiesView(nullptr),
    m_trackTreeModel(nullptr),
    m_fileSystemModel(nullptr),
    m_dataWidgetMapper(nullptr),
    m_trackFullView(nullptr),
    m_trackPlaybackThread(),
    m_trackPlaybackControl(0)
{
    ui->setupUi(this);

    // setup models
    m_fileSystemModel= new QFileSystemModel(this);
    m_fileSystemModel->setRootPath("/Users/keeferdavies/dev/");
    m_trackTreeModel = new PGTATrackTreeModel(this);

    // remove title bar from all dock widgets
    ui->TopPanel->setTitleBarWidget(new QWidget());

    // setup dockables
    m_propertiesDock = new PGTADockable(tr("Properties"), this);
    m_propertiesView = new PGTAPropertiesView(this);
    m_propertiesDock->setWidget(m_propertiesView);

    m_trackDock = new PGTADockable(tr("Track Definition"), this);
    m_trackView = new PGTATrackView(this);
    m_trackView->SetTreeViewModel(m_trackTreeModel);
    m_trackView->SetPropertiesView(m_propertiesView);
    m_trackDock->setWidget(m_trackView);

    // add dockables
    addDockWidget(Qt::RightDockWidgetArea, m_trackDock);
    addDockWidget(Qt::RightDockWidgetArea, m_propertiesDock);


    // set modles
    ui->FileSystemView->setModel(m_fileSystemModel);
    //ui->TrackTreeView->setModel(m_trackTreeModel);

    QModelIndex idx = m_fileSystemModel->index("/Users/keeferdavies/dev/tmp/Sample Project");
    ui->FileSystemView->setRootIndex(idx);

    // only show first column file system view
    for (int i = 1; i < m_fileSystemModel->columnCount(); ++i)
    {
        ui->FileSystemView->hideColumn(i);
    }

    // only show first column track tree view
    for (int i = 1; i < m_trackTreeModel->columnCount(); ++i)
    {
        ui->TrackTreeView->hideColumn(i);
    }

    // setup signals and slots for track view
    connect(ui->insertSampleAction, SIGNAL(triggered()), m_trackView, SLOT(slotInsertSample()));
    connect(ui->insertGroupAction, SIGNAL(triggered()), m_trackView, SLOT(slotInsertGroup()));
    connect(ui->removeTrackItemAction, SIGNAL(triggered()), m_trackView, SLOT(slotRemoveTrackItem()));

    connect(ui->viewFullModelAction, SIGNAL(triggered()), this, SLOT(viewFullModel()));

    // playback controls
    connect(ui->PlayButton, SIGNAL(clicked()), this, SLOT(playTrack()));
    connect(ui->PauseButton, SIGNAL(clicked()), this, SLOT(pauseTrack()));
    connect(ui->StopButton, SIGNAL(clicked()), this, SLOT(stopTrack()));

    // volume slider
    //connect(ui->EditVolume, SIGNAL(sliderMoved(int)), this, SLOT(showSliderTooltip(int)));


    ui->TrackTreeView->setDropIndicatorShown(true);
    ui->TrackTreeView->setDefaultDropAction(Qt::MoveAction);
    ui->statusBar->showMessage("Ready");
}

MainWindow::~MainWindow()
{
    // stop playback thread if playing
    m_trackPlaybackControl = 2;
    if (m_trackPlaybackThread.joinable())
    {
        m_trackPlaybackThread.join();
    }

    // delete dockables
    // TODO : remove order dependency
    delete m_propertiesView;
    delete m_propertiesDock;

    delete m_trackView;
    delete m_trackDock;

    delete m_trackTreeModel;
    delete m_fileSystemModel;
    delete m_trackFullView;
    delete ui;
}

static void PGTAPlayTrack(std::string trackFile, std::atomic<int> &trackPlaybackControl, std::string &message)
{
    try
    {
        PGTATestCommon::PlayTrack(trackFile, trackPlaybackControl, message);
        qDebug("%s", message.c_str());
    }
    catch(...)
    {
    }
}

void MainWindow::playTrack()
{
    m_trackPlaybackControl = 2;
    if (m_trackPlaybackThread.joinable())
    {
        m_trackPlaybackThread.join();
    }
    m_trackPlaybackControl = 0;
    if (m_trackTreeModel->getIsDirty())
    {
        QMessageBox msgBox;
        msgBox.setText("The track has been modified.");
        msgBox.setInformativeText("Do you want to save your changes?");
        msgBox.setIconPixmap(QPixmap(":/icons/icon.icns"));
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);
        int ret = msgBox.exec();
        switch (ret) {
          case QMessageBox::Save:
              on_actionSave_triggered();
              break;
          case QMessageBox::Cancel:
              return;
          default:
              break;
        }
    }
    m_trackPlaybackThread = std::thread(PGTAPlayTrack, m_trackTreeModel->getFilePath().toStdString(),
                std::ref(m_trackPlaybackControl), std::ref(m_playbackMessage));
}

void MainWindow::pauseTrack()
{
    m_trackPlaybackControl = 1;
}

void MainWindow::stopTrack()
{
    m_trackPlaybackControl = 2;
}

void MainWindow::viewFullModel()
{
    if (!m_trackFullView)
    {
        m_trackFullView = new PGTATreeView();
    }
    m_trackFullView->setModel(m_trackTreeModel);
    m_trackFullView->show();
}

void MainWindow::updateStatusBar(QString message, StatusBarState state)
{
    switch (state)
    {
    case StatusBarState_READY:
        ui->statusBar->setStyleSheet("QStatusBar{background: #5C1E6E; color: #FFFFFF;}");
        break;
    case StatusBarState_OK:
        ui->statusBar->setStyleSheet("QStatusBar{background: #137CCA; color: #FFFFFF;}");
        break;
    case StatusBarState_ERROR:
        ui->statusBar->setStyleSheet("QStatusBar{background: #C85217; color: #FFFFFF;}");
        break;
    }
    ui->statusBar->showMessage(message);
}

void MainWindow::onCustomContextMenu(const QPoint &point)
{
    QModelIndex index = ui->TrackTreeView->indexAt(point);
    QPoint globalPos = ui->TrackTreeView->mapToGlobal(point);
    PGTATrackTreeModel *model = static_cast<PGTATrackTreeModel*>(ui->TrackTreeView->model());
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

void MainWindow::insertGroup()
{
    PGTATrackTreeModel *model = static_cast<PGTATrackTreeModel*>(ui->TrackTreeView->model());
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
    QModelIndex child = model->index(index.row()+1, 0, index.parent());
    model->setIsGroup(child);
    for (int column = 0; column < model->columnCount(index.parent()); ++column)
    {
        child = model->index(index.row()+1, column, index.parent());
        switch (column)
        {
            case PGTATrackTreeModel::GroupColumn_Name :
                model->setData(child, QVariant("[Group Name]"), Qt::EditRole);
                break;
            case PGTATrackTreeModel::GroupColumn_UUID :
                model->setData(child, model->getUuid(child), Qt::EditRole);
                break;
            default:
                break;
        }
    }
}

void MainWindow::removeTrackItem()
{
    QModelIndex index = ui->TrackTreeView->selectionModel()->currentIndex();

    if (!index.isValid())
    {
        return;
    }

    PGTATrackTreeModel *model = static_cast<PGTATrackTreeModel*>(ui->TrackTreeView->model());
    if(model->removeRow(index.row(), index.parent()))
    {
        ui->TrackTreeView->selectionModel()->clear();
    }
}

void MainWindow::insertSample()
{
    QModelIndex selectedIndex = ui->TrackTreeView->selectionModel()->currentIndex();
    PGTATrackTreeModel *model = static_cast<PGTATrackTreeModel*>(ui->TrackTreeView->model());

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
        switch (column)
        {
            case PGTATrackTreeModel::SampleColumn_Name :
                model->setData(child, QVariant("[Sample Name]"), Qt::EditRole);
                break;
            case PGTATrackTreeModel::SampleColumn_GroupUUID :
                model->setData(child, model->getUuid(index), Qt::EditRole);
                break;
            case PGTATrackTreeModel::SampleColumn_Volume :
                model->setData(child, QVariant(0), Qt::EditRole);
                break;
            default:
                model->setData(child, QVariant("[No data]"), Qt::EditRole);
                break;
        }

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
    qDebug("Saving File");
    std::string filePath = m_trackTreeModel->getFilePath().toStdString();

    if (filePath.length() == 0)
    {
        QString file = QFileDialog::getSaveFileName(this, tr("Save Track File"), "",tr("Track files (*.track)"));
        if (file.isEmpty())
        {
            updateStatusBar("Error saving track file.", StatusBarState_ERROR);
            return;
        }
        filePath = file.toStdString();
    }

    if (FlatBufferTrackWriter::WriteTrack(m_trackTreeModel, filePath))
    {
        m_trackTreeModel->setFilePath(QString::fromStdString(filePath));
        m_trackTreeModel->setIsDirty(false);
        updateStatusBar("Track file saved successfully.", StatusBarState_OK);
    }
}

void MainWindow::on_actionOpen_triggered()
{
    QStringList fileList = QFileDialog::getOpenFileNames(this, tr("Open Track File"),"",tr("Track files (*.track)"));
    if (fileList.isEmpty())
    {
        ui->statusBar->showMessage("Error opening file.");
        return;
    }
    std::string fileName = fileList.first().toStdString();

    if (m_trackTreeModel)
    {
        delete m_trackTreeModel;
    }

    if (m_dataWidgetMapper)
    {
        delete m_dataWidgetMapper;
    }

    m_trackTreeModel = new PGTATrackTreeModel(this);
    m_dataWidgetMapper = new QDataWidgetMapper(this);

    std::string buffer;
    if (!FileUtils::ReadBinaryFileToString(fileName, buffer))
    {
        qDebug("Error reading from file.");
        updateStatusBar("Error reading from file..", StatusBarState_ERROR);
        return;
    }

    // initialize track model
    if(!FlatbufferTrackLoader::LoadTrack(buffer.c_str(), buffer.length(), m_trackTreeModel))
    {
        updateStatusBar("Error reading from file.", StatusBarState_ERROR);
    }
    else
    {
        updateStatusBar("File opened successfully.", StatusBarState_OK);
        m_trackTreeModel->setFilePath(QString::fromStdString(fileName));
    }

    m_trackView->SetTreeViewModel(m_trackTreeModel);
    ui->TrackTreeView->setModel(m_trackTreeModel);
}

