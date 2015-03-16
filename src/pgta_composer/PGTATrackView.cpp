
#include <QPaintEvent>
#include <QPainter>
#include "PGTATrackView.h"
#include "ui_PGTATrackView.h"
#include "PGTATreeView.h"

PGTATrackView::PGTATrackView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PGTATrackView),
    m_treeView(nullptr)
{
    ui->setupUi(this);
    m_treeView = new PGTATreeView(this);
    m_treeView->header()->hide();
    ui->mainLayout->addWidget(m_treeView);
}

PGTATrackView::~PGTATrackView()
{
    delete m_treeView;
    delete ui;
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
    m_treeView->setModel(model);
    // only show first column track tree view
    for (int i = 1; i < model->columnCount(); ++i)
    {
        m_treeView->hideColumn(i);
    }
}
