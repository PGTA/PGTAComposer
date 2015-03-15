
#pragma once

#include <QVariant>
#include <QVector>
#include <QList>

class PGTALibraryItem
{
public:
    explicit PGTALibraryItem(const QVector<QVariant> &data, PGTALibraryItem *parent = nullptr,
                             const bool isFolder = false);
    ~PGTALibraryItem();

    bool IsFolder() const;
    PGTALibraryItem *GetParent() const;
    void SetIsFolder(bool isFolder);

    int ChildCount() const;
    void AddChild(PGTALibraryItem *item);
    int GetChildRow(PGTALibraryItem *child) const;

    PGTALibraryItem *GetChild(int row) const;
    bool InsertChildren(int position, int count, int columns);
    bool RemoveChildren(int position, int count);

    int GetRow() const;
    int ColumnCount() const;
    QVariant GetData(int column) const;
    bool SetData(int column, const QVariant &value);

private:
    QList<PGTALibraryItem*> m_childItems;
    QVector<QVariant> m_itemData;
    PGTALibraryItem *m_parent;
    bool m_isFolder;
};
