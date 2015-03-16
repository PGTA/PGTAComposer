
#pragma once

#include <memory>
#include <QWidget>
#include <QAbstractItemModel>
#include <QPaintEvent>

class PGTATreeView;

namespace Ui {
class PGTATrackView;
}

class PGTATrackView : public QWidget
{
    Q_OBJECT

public:
    explicit PGTATrackView(QWidget *parent = 0);
    ~PGTATrackView();

public:
    void SetTreeViewModel(QAbstractItemModel *model);

private slots:

private:
    void paintEvent(QPaintEvent *) override;
private:
    Ui::PGTATrackView *ui;
    PGTATreeView* m_treeView;
};

