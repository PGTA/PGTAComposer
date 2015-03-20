
#include <QPaintEvent>
#include <QMenu>
#include <QPainter>
#include <QPoint>
#include "PGTATreeView.h"
#include "PGTALibraryView.h"
#include "ui_PGTALibraryView.h"

PGTALibraryView::PGTALibraryView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PGTALibraryView),
    m_treeView(nullptr),
    m_libraryModel(nullptr)
{
    ui->setupUi(this);
    m_treeView = new PGTATreeView(this);
    m_treeView->header()->hide();
    ui->mainLayout->addWidget(m_treeView);
    SetupAddButtonMenu();
    ConnectSignals();
}

PGTALibraryView::~PGTALibraryView()
{
    delete ui;
    delete m_treeView;
    delete m_libraryModel;
}

void PGTALibraryView::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void PGTALibraryView::ConnectSignals()
{

}

void PGTALibraryView::SetupAddButtonMenu()
{
    // setup tool button context menu for add sample/group button
    QMenu *menu = new QMenu();
    QAction *addMedia = menu->addAction("Add Media");
    //connect(addGroup, SIGNAL(triggered()), this, SLOT(slotInsertGroup()));
    QAction *addTrack = menu->addAction("Add Track");
    //connect(addSample, SIGNAL(triggered()), this, SLOT(slotInsertSample()));
    ui->addLibraryItemButton->setPopupMode(QToolButton::InstantPopup);
    ui->addLibraryItemButton->setMenu(menu);
}

void PGTALibraryView::slotAddMedia()
{

}

void PGTALibraryView::slotRemoveMedia()
{

}

void PGTALibraryView::slotLibraryMediaClicked(QModelIndex index)
{

}

