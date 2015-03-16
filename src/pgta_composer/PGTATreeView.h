
#pragma once

#include <QTreeView>
#include <QMouseEvent>

class PGTATreeView : public QTreeView
{
public:
    PGTATreeView(QWidget *parent = nullptr);
    virtual ~PGTATreeView();
private:
    virtual void mousePressEvent(QMouseEvent *event);
};

