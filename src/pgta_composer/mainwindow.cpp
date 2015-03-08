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
#include <QUuid>
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

    // setup signals and slots
    connect(ui->TrackTreeView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
        this, SLOT(treeViewRowColChange(QModelIndex)));
    connect(ui->TrackTreeView, SIGNAL(customContextMenuRequested(const QPoint &)), this,
            SLOT(onCustomContextMenu(const QPoint &)));
    connect(ui->insertSampleAction, SIGNAL(triggered()), this, SLOT(insertSample()));
    connect(ui->insertGroupAction, SIGNAL(triggered()), this, SLOT(insertGroup()));
    connect(ui->removeTrackItemAction, SIGNAL(triggered()), this, SLOT(removeTrackItem()));
    connect(ui->removeTrackItemButton, SIGNAL(clicked()), this, SLOT(removeTrackItem()));

    // setup tool button context menu for add sample/group button
    QMenu *menu = new QMenu();
    QAction *addGroup = menu->addAction("Add Group");
    connect(addGroup, SIGNAL(triggered()), this, SLOT(insertGroup()));
    QAction *addSample = menu->addAction("Add Sample");
    connect(addSample, SIGNAL(triggered()), this, SLOT(insertSample()));
    ui->addTrackItemButton->setPopupMode(QToolButton::InstantPopup);
    ui->addTrackItemButton->setMenu(menu);

    // right panel
    connect(ui->toggleRightPanelAction, SIGNAL(triggered()), this, SLOT(toggleRightPanel()));
    connect ( ui->ToggleRightPanel, SIGNAL(clicked()), this, SLOT(toggleRightPanel()));

    // left panel
    connect(ui->toggleLeftPanelAction, SIGNAL(triggered()), this, SLOT(toggleLeftPanel()));
    connect ( ui->ToggleLeftPanel, SIGNAL(clicked()), this, SLOT(toggleLeftPanel()));


    ui->TrackTreeView->setDropIndicatorShown(true);
    ui->TrackTreeView->setDefaultDropAction(Qt::MoveAction);
    ui->statusBar->showMessage("Ready");

    ui->PlayButton->setIcon(QIcon(":/img/play_64x64.png"));
    ui->PauseButton->setIcon(QIcon(":/img/pause_64x64.png"));
    ui->StopButton->setIcon(QIcon(":/img/stop_64x64.png"));
    ui->ToggleLeftPanel->setIcon(QIcon(":/img/leftpanelselected_64x64.png"));
    ui->ToggleRightPanel->setIcon(QIcon(":/img/rightpanelselected_64x64.png"));
    ui->addTrackItemButton->setIcon(QIcon(":/img/plus_24x24.png"));
    ui->removeTrackItemButton->setIcon(QIcon(":/img/minus_24x24.png"));

}

MainWindow::~MainWindow()
{
    delete m_dataWidgetMapper;
    delete m_trackTreeModel;
    delete m_fileSystemModel;
    delete ui;
}

void MainWindow::toggleRightPanel()
{
    if (ui->RightPanel->isHidden())
    {
        ui->ToggleRightPanel->setIcon(QIcon(":/img/rightpanelselected_64x64.png"));
        ui->toggleRightPanelAction->setText("Hide Right Panel");
        ui->RightPanel->show();
        return;
    }
    ui->ToggleRightPanel->setIcon(QIcon(":/img/rightpanel_64x64.png"));
    ui->toggleRightPanelAction->setText("Show Right Panel");
    ui->RightPanel->hide();
    return;
}

void MainWindow::toggleLeftPanel()
{
    if (ui->LeftPanel->isHidden())
    {
        ui->ToggleLeftPanel->setIcon(QIcon(":/img/leftpanelselected_64x64.png"));
        ui->toggleLeftPanelAction->setText("Hide Left Panel");
        ui->LeftPanel->show();
        return;
    }
    ui->ToggleLeftPanel->setIcon(QIcon(":/img/leftpanel_64x64.png"));
    ui->toggleLeftPanelAction->setText("Show Left Panel");
    ui->LeftPanel->hide();
    return;
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
    QModelIndex child = model->index(index.row()+1, 0, index.parent());
    model->setIsGroup(child);
    for (int column = 0; column < model->columnCount(index.parent()); ++column)
    {
        child = model->index(index.row()+1, column, index.parent());
        switch (column)
        {
            case TrackTreeModel::GroupColumn_Name :
                model->setData(child, QVariant("[Group Name]"), Qt::EditRole);
                break;
            case TrackTreeModel::GroupColumn_UUID :
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
        switch (column)
        {
            case TrackTreeModel::SampleColumn_Name :
                model->setData(child, QVariant("[Sample Name]"), Qt::EditRole);
                break;
            case TrackTreeModel::SampleColumn_GroupUUID :
                model->setData(child, model->getUuid(index), Qt::EditRole);
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

