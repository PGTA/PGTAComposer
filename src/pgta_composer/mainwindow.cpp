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
    m_trackPlaybackControl(PlaybackControl::Stop),
    m_volumeMultiplier(70),
    m_mediaPlayer(nullptr)
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
    //m_libraryModel->setFilter(QDir::NoDotAndDotDot | QDir::Files);
    QStringList filters;
        filters << "*.wav" << "*.track";
    m_libraryModel->setNameFilters(filters);
    m_libraryModel->setNameFilterDisables(false);
    m_libraryView->setModel(m_libraryModel);
    m_libraryView->setDragEnabled(true);
    m_libraryView->setDragDropMode(QAbstractItemView::DragDrop);
    m_libraryView->setDefaultDropAction(Qt::CopyAction);
    m_libraryDock->setWidget(m_libraryView);

    // add dockables
    addDockWidget(Qt::LeftDockWidgetArea, m_libraryDock);
    addDockWidget(Qt::RightDockWidgetArea, m_trackDock);
    addDockWidget(Qt::RightDockWidgetArea, m_propertiesDock);

    m_libraryModel->setRootPath(QDir::currentPath());


    QModelIndex idx = m_libraryModel->index(QDir::currentPath());
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
    connect(ui->actionNew, SIGNAL(triggered()), this, SLOT(slotNewTrack()));

    connect(ui->ue4ExportAction, SIGNAL(triggered()), this, SLOT(slotUe4Export()));

    connect(ui->viewFullModelAction, SIGNAL(triggered()), this, SLOT(viewFullModel()));

    // playback controls
    connect(ui->PlayButton, SIGNAL(clicked()), this, SLOT(playTrack()));
    connect(ui->PauseButton, SIGNAL(clicked()), this, SLOT(pauseTrack()));
    connect(ui->StopButton, SIGNAL(clicked()), this, SLOT(stopTrack()));

    // volume slider
    connect(ui->VolumeSlider, SIGNAL(valueChanged(int)), this, SLOT(slotUpdateVolumeMultiplier(int)));

    // library
    connect(m_libraryView, SIGNAL(clicked(QModelIndex)), this, SLOT(slotLibraryMediaClicked(QModelIndex)));

    ui->statusBar->showMessage("Ready");
}

MainWindow::~MainWindow()
{
    // stop playback thread if playing
    m_trackPlaybackControl = PlaybackControl::Stop;
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
    delete m_mediaPlayer;
    delete ui;
}

void MainWindow::PlaySample(const QString &filePath)
{
    if (!m_mediaPlayer)
    {
        m_mediaPlayer = new QMediaPlayer();
    }
    m_mediaPlayer->stop();
    QUrl url = QUrl::fromLocalFile(filePath);
    if (m_mediaPlayer->currentMedia().canonicalUrl() == url)
    {
        m_mediaPlayer->setMedia(QMediaContent());
        return;
    }
    m_mediaPlayer->setMedia(url);
    m_mediaPlayer->play();
}

void MainWindow::slotUe4Export()
{
    QString file = QFileDialog::getSaveFileName(this, tr("Save Track File"), QDir::current().absolutePath(),
                                                tr("Track files (*.track)"));
    if (file.isEmpty())
    {
        updateStatusBar("Track file was not saved.", StatusBarState_ERROR);
        return;
    }
    std::string filePath = file.toStdString();

    if (FlatBufferTrackWriter::WriteTrack(m_trackTreeModel, filePath, true))
    {
        updateStatusBar("Track file exported for UE4 successfully.", StatusBarState_OK);
    }
}

void MainWindow::slotLibraryMediaClicked(QModelIndex index)
{
    QFileSystemModel *model = static_cast<QFileSystemModel*>(m_libraryView->model());

    const QString type = model->type(index);
    const QString filePath = model->filePath(index);

    if (type == "track File")
    {
        if (SaveTrack())
        {
            OpenTrack(filePath.toStdString());
        }
    }
    else if (type == "wav File")
    {
        PlaySample(filePath);
    }
}

void MainWindow::slotUpdateVolumeMultiplier(int value)
{
    m_volumeMultiplier = static_cast<uint8_t>(value);
    if (m_mediaPlayer)
    {
        m_mediaPlayer->setVolume(value);
    }
}

static void PGTAPlayTrack(const std::string trackFile, const std::atomic<PlaybackControl> &trackPlaybackControl,
                          const std::atomic<uint8_t> &volumeMultiplier, const uint16_t beatsPerMinute,
                          std::string &message)
{
    try
    {
        PGTATestCommon::PlayTrack(trackFile, trackPlaybackControl, volumeMultiplier, beatsPerMinute, message);
        qDebug("%s", message.c_str());
    }
    catch(...)
    {
    }
}

void MainWindow::playTrack()
{
    if (m_trackPlaybackControl == PlaybackControl::Pause)
    {
        m_trackPlaybackControl = PlaybackControl::Play;
        return;
    }

    m_trackPlaybackControl = PlaybackControl::Stop;
    if (m_trackPlaybackThread.joinable())
    {
        m_trackPlaybackThread.join();
    }
    m_trackPlaybackControl = PlaybackControl::Play;
    if (!SaveTrack())
    {
        return;
    }
    const uint16_t beatsPerMinute =  static_cast<uint16_t>(ui->BeatsPerMin->value());
    m_trackPlaybackThread = std::thread(PGTAPlayTrack, m_trackTreeModel->getFilePath().toStdString(),
                                        std::ref(m_trackPlaybackControl), std::ref(m_volumeMultiplier), beatsPerMinute,
                                        std::ref(m_playbackMessage));
}

void MainWindow::pauseTrack()
{
    m_trackPlaybackControl = PlaybackControl::Pause;
}

void MainWindow::stopTrack()
{
    m_trackPlaybackControl = PlaybackControl::Stop;
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
        QString file = QFileDialog::getSaveFileName(this, tr("Save Track File"), QDir::current().absolutePath(),
                                                    tr("Track files (*.track)"));
        if (file.isEmpty())
        {
            updateStatusBar("Track file was not saved.", StatusBarState_ERROR);
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

void MainWindow::OpenTrack(const std::string &fileName)
{
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

void MainWindow::on_actionOpen_triggered()
{
    QStringList fileList = QFileDialog::getOpenFileNames(this, tr("Open Track File"),"",tr("Track files (*.track)"));
    if (fileList.isEmpty())
    {
        ui->statusBar->showMessage("Error opening file.");
        return;
    }
    std::string fileName = fileList.first().toStdString();
    OpenTrack(fileName);
}

bool MainWindow::SaveTrack()
{
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
              return false;
          default:
              break;
        }
    }
    return true;
}

void MainWindow::slotNewTrack()
{
    SaveTrack();

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

    m_trackView->SetTreeViewModel(m_trackTreeModel);
}

