
#pragma once

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <memory>
#include "PGTALibraryItem.h"

class PGTALibraryModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    PGTALibraryModel(QObject *parent = nullptr);
    ~PGTALibraryModel();
public:
    QModelIndex parent(const QModelIndex &index) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant data(const QModelIndex &index, int role) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    // insert & remove operations
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value,
                       int role = Qt::EditRole) override;

    bool isFolder(const QModelIndex &index) const;
private:
    PGTALibraryItem *getItem(const QModelIndex &index) const;
private:
    std::unique_ptr<PGTALibraryItem> m_rootItem;
};
