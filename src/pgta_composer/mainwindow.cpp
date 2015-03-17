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

#include <QMediaPlayer>
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
    m_libraryDock(nullptr),
    m_libraryView(nullptr),
    m_trackFullView(nullptr),
    m_trackTreeModel(nullptr),
    m_libraryModel(nullptr),
    m_dataWidgetMapper(nullptr),

    m_trackPlaybackThread(),
    m_trackPlaybackControl(0)
{
    ui->setupUi(this);

    // setup models
    m_trackTreeModel = new PGTATrackTreeModel(this);
    m_libraryModel = new QFileSystemModel(this);

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

    m_libraryDock = new PGTADockable(tr("Library"), this);
    m_libraryView = new QTreeView(this);
    m_libraryView->setModel(m_libraryModel);
    m_libraryDock->setWidget(m_libraryView);

    // add dockables
    addDockWidget(Qt::LeftDockWidgetArea, m_libraryDock);
    addDockWidget(Qt::RightDockWidgetArea, m_trackDock);
    addDockWidget(Qt::RightDockWidgetArea, m_propertiesDock);

    m_libraryModel->setRootPath("/Users/keeferdavies/dev/");
    QModelIndex idx = m_libraryModel->index("/Users/keeferdavies/dev/tmp/Sample Project");
    m_libraryView->setRootIndex(idx);
    m_libraryView->header()->hide();

    // only show first column file system view
    for (int i = 1; i < m_libraryModel->columnCount(); ++i)
    {
        m_libraryView->hideColumn(i);
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

    // gain slider
    //connect(ui->EditGain, SIGNAL(sliderMoved(int)), this, SLOT(showSliderTooltip(int)));

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

    delete m_libraryView;
    delete m_libraryDock;

    delete m_trackTreeModel;
    delete m_libraryModel;
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
}

