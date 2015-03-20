
#pragma once

#include <QString>
#include <QWidget>
#include <QmodelIndex>
#include <QPaintEvent>
#include <QFileSystemModel>
#include "PGTATreeView.h"

namespace Ui {
class PGTALibraryView;
}

class PGTALibraryView : public QWidget
{
    Q_OBJECT

public:
    explicit PGTALibraryView(QWidget *parent = 0);
    ~PGTALibraryView();

public:
    void SetLibraryDirectory(const QString &dir);
    void SetupAddButtonMenu();

private slots:
    void slotAddMedia();
    void slotRemoveMedia();
    void slotLibraryMediaClicked(QModelIndex index);

private:
    void paintEvent(QPaintEvent *) override;
    void ConnectSignals();

private:
    Ui::PGTALibraryView *ui;
    PGTATreeView *m_treeView;
    QFileSystemModel *m_libraryModel;
};

