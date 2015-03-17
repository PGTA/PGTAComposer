
#include <QMouseEvent>
#include <QModelIndex>
#include <QItemSelectionModel>
#include "PGTATreeView.h"

PGTATreeView::PGTATreeView(QWidget *parent) :
    QTreeView(parent)
{
    setContextMenuPolicy(Qt::CustomContextMenu);
    setDragDropMode(QAbstractItemView::DragDrop);
    setDefaultDropAction(Qt::MoveAction);
    setDragEnabled(true);
    setDropIndicatorShown(true);
}

PGTATreeView::~PGTATreeView()
{
}

void PGTATreeView::mousePressEvent(QMouseEvent *event)
    {
        QModelIndex item = indexAt(event->pos());
        bool selected = selectionModel()->isSelected(indexAt(event->pos()));
        QTreeView::mousePressEvent(event);
        Qt::MouseButton button = event->button();
        if (item.row() == -1 && item.column() == -1)
        {
            clearSelection();
            const QModelIndex index;
            selectionModel()->setCurrentIndex(index, QItemSelectionModel::Select);
        }
    }

