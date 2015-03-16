
#include <QMouseEvent>
#include <QModelIndex>
#include <QItemSelectionModel>
#include "PGTATreeView.h"

PGTATreeView::PGTATreeView(QWidget *parent) :
    QTreeView(parent)
{
}

PGTATreeView::~PGTATreeView()
{
}

void PGTATreeView::mousePressEvent(QMouseEvent *event)
    {
        QModelIndex item = indexAt(event->pos());
        bool selected = selectionModel()->isSelected(indexAt(event->pos()));
        QTreeView::mousePressEvent(event);
        if ((item.row() == -1 && item.column() == -1) || selected)
        {
            clearSelection();
            const QModelIndex index;
            selectionModel()->setCurrentIndex(index, QItemSelectionModel::Select);
        }
    }
