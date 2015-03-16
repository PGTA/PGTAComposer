
#pragma once

#include <QDockWidget>

class PGTADockable : public QDockWidget
{
public:
    PGTADockable(const QString & title, QWidget * parent = 0, Qt::WindowFlags flags = 0);
    virtual ~PGTADockable();
};
