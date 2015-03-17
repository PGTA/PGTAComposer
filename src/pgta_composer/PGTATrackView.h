
#pragma once

#include <memory>
#include <QWidget>
#include <QAbstractItemModel>
#include <QDataWidgetMapper>
#include <QPaintEvent>
#include "PGTAPropertiesView.h"

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
    void SetPropertiesView(PGTAPropertiesView *propertiesView);

private slots:
    void treeViewRowColChange(const QModelIndex &index);
    void onCustomContextMenu(const QPoint &point);
    void slotInsertGroup();
    void slotInsertSample();
    void slotRemoveTrackItem();
    void slotMeasuredInBeats(bool isChecked);

private:
    void paintEvent(QPaintEvent *) override;
    void ConnectDataWidgetMapper(QAbstractItemModel *model);
    void ConnectSignals();
    void SetupAddButtonMenu();
private:
    Ui::PGTATrackView *ui;
    PGTATreeView* m_treeView;
    QDataWidgetMapper *m_dataWidgetMapper;
    PGTAPropertiesView *m_propertiesView;
};

