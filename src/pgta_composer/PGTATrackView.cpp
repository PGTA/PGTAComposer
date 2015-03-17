
#include <QPaintEvent>
#include <QPainter>
#include <QMenu>
#include <QPoint>
#include "PGTATrackView.h"
#include "ui_PGTATrackView.h"
#include "PGTATreeView.h"
#include "PGTAPropertiesView.h"
#include "ui_PGTAPropertiesView.h"
#include "PGTATrackTreeModel.h"

PGTATrackView::PGTATrackView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PGTATrackView),
    m_treeView(nullptr),
    m_dataWidgetMapper(nullptr),
    m_propertiesView(nullptr)
{
    ui->setupUi(this);
    m_treeView = new PGTATreeView(this);
    m_treeView->header()->hide();
    ui->mainLayout->addWidget(m_treeView);
    SetupAddButtonMenu();
    ConnectSignals();
}

PGTATrackView::~PGTATrackView()
{
    delete m_treeView;
    delete m_dataWidgetMapper;
    delete ui;
}

void PGTATrackView::ConnectSignals()
{
    // connect singnals and slots
    connect(m_treeView, SIGNAL(customContextMenuRequested(QPoint)), this,
            SLOT(onCustomContextMenu(const QPoint &)));
    connect(ui->removeTrackItemButton, SIGNAL(clicked()), this, SLOT(slotRemoveTrackItem()));
}

void PGTATrackView::SetupAddButtonMenu()
{
    // setup tool button context menu for add sample/group button
    QMenu *menu = new QMenu();
    QAction *addGroup = menu->addAction("Add Group");
    connect(addGroup, SIGNAL(triggered()), this, SLOT(slotInsertGroup()));
    QAction *addSample = menu->addAction("Add Sample");
    connect(addSample, SIGNAL(triggered()), this, SLOT(slotInsertSample()));
    ui->addTrackItemButton->setPopupMode(QToolButton::InstantPopup);
    ui->addTrackItemButton->setMenu(menu);
}

void PGTATrackView::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void PGTATrackView::SetTreeViewModel(QAbstractItemModel *model)
{
    if (!model)
    {
        return;
    }

    m_treeView->setModel(model);
    // only show first column track tree view
    for (int i = 1; i < model->columnCount(); ++i)
    {
        m_treeView->hideColumn(i);
    }

    ConnectDataWidgetMapper(model);
}

void PGTATrackView::SetPropertiesView(PGTAPropertiesView *propertiesView)
{
    m_propertiesView = propertiesView;
    if (!m_treeView)
    {
        return;
    }

    QAbstractItemModel *model = m_treeView->model();
    if (model)
    {
        ConnectDataWidgetMapper(model);
    }
}

void PGTATrackView::ConnectDataWidgetMapper(QAbstractItemModel *model)
{
    if (!m_propertiesView || !model)
    {
        return;
    }

    if (m_dataWidgetMapper)
    {
        delete m_dataWidgetMapper;
    }

    Ui::PGTAPropertiesView *propertiesUi = m_propertiesView->GetUi();
    m_dataWidgetMapper = new QDataWidgetMapper(this);
    m_dataWidgetMapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
    m_dataWidgetMapper->setModel(model);
    m_dataWidgetMapper->addMapping(propertiesUi->EditName, PGTATrackTreeModel::SampleColumn_Name);
    m_dataWidgetMapper->addMapping(propertiesUi->EditDefaultFile, PGTATrackTreeModel::SampleColumn_DefaultFile);
    m_dataWidgetMapper->addMapping(propertiesUi->EditStartTime, PGTATrackTreeModel::SampleColumn_StartTime);
    m_dataWidgetMapper->addMapping(propertiesUi->EditPeriod, PGTATrackTreeModel::SampleColumn_Period);
    m_dataWidgetMapper->addMapping(propertiesUi->EditDeviation, PGTATrackTreeModel::SampleColumn_PeriodDeviation);
    m_dataWidgetMapper->addMapping(propertiesUi->EditProbability, PGTATrackTreeModel::SampleColumn_Probability);
    m_dataWidgetMapper->addMapping(propertiesUi->EditVolume, PGTATrackTreeModel::SampleColumn_Volume);
    connect(m_treeView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
        this, SLOT(treeViewRowColChange(QModelIndex)));
}

void PGTATrackView::treeViewRowColChange(const QModelIndex &index)
{
    if (!m_treeView)
    {
        return;
    }

    m_dataWidgetMapper->setRootIndex(index.parent());
    m_dataWidgetMapper->setCurrentModelIndex(index);
    if (static_cast<PGTATrackTreeModel*>(m_treeView->model())->isGroup(index))
    {
        m_propertiesView->SetSamplePropertiesShown(false);
    }
    else
    {
        m_propertiesView->SetSamplePropertiesShown(true);
    }
}

void PGTATrackView::onCustomContextMenu(const QPoint &point)
{
    if (!m_treeView)
    {
        return;
    }

    QModelIndex index = m_treeView->indexAt(point);
    QPoint globalPos = m_treeView->mapToGlobal(point);
    PGTATrackTreeModel *model = static_cast<PGTATrackTreeModel*>(m_treeView->model());
    QMenu myMenu;

    QAction *insertSampleAction = myMenu.addAction("Insert Sample");
    connect(insertSampleAction, SIGNAL(triggered()), this, SLOT(slotInsertSample()));

    if (index.isValid())
    {
        QString removeText = "Remove Sample";
        if (model->isGroup(index))
        {
            removeText = "Remove Group";
        }
        QAction *removeTrackItemAction = myMenu.addAction(removeText);
        connect(removeTrackItemAction, SIGNAL(triggered()), this, SLOT(slotRemoveTrackItem()));
    }
    else
    {
        QAction *insertGroupAction = myMenu.addAction("Insert Group");
        connect(insertGroupAction, SIGNAL(triggered()), this, SLOT(slotInsertGroup()));
    }
    myMenu.exec(globalPos);
}

void PGTATrackView::slotInsertGroup()
{
    if (!m_treeView)
    {
        return;
    }

    PGTATrackTreeModel *model = static_cast<PGTATrackTreeModel*>(m_treeView->model());
    QModelIndex rootIndex = m_treeView->rootIndex();
    QModelIndex index = m_treeView->selectionModel()->currentIndex();
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

void PGTATrackView::slotInsertSample()
{
    if (!m_treeView)
    {
        return;
    }

    QModelIndex selectedIndex = m_treeView->selectionModel()->currentIndex();
    PGTATrackTreeModel *model = static_cast<PGTATrackTreeModel*>(m_treeView->model());

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
    m_treeView->selectionModel()->setCurrentIndex(model->index(position, 0, index),
                                            QItemSelectionModel::ClearAndSelect);
}

void PGTATrackView::slotRemoveTrackItem()
{
    if (!m_treeView)
    {
        return;
    }

    QModelIndex index = m_treeView->selectionModel()->currentIndex();
    if (!index.isValid())
    {
        return;
    }

    PGTATrackTreeModel *model = static_cast<PGTATrackTreeModel*>(m_treeView->model());
    if(model->removeRow(index.row(), index.parent()))
    {
        m_treeView->selectionModel()->clear();
        //clearSampleProperties();
    }
}




