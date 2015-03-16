
#include <QDockWidget>
#include "PGTADockable.h"

PGTADockable::PGTADockable(const QString &title, QWidget *parent, Qt::WindowFlags flags) :
    QDockWidget(title, parent, flags)
{
    setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    setFeatures(QDockWidget::DockWidgetMovable);
}

PGTADockable::~PGTADockable()
{
}
