
#pragma once

#include <QVariant>
#include <QUuid>
#include <QVector>
#include <QList>

class PGTATrackItem
{

public:
    explicit PGTATrackItem(const QVector<QVariant> &data, PGTATrackItem *parent = nullptr,
                       const QUuid &uuid = QUuid(), const bool isGroup = false);
    ~PGTATrackItem();

public:
    void AddChild(PGTATrackItem *item);
    bool InsertChildren(int position, int count, int columns);
    bool RemoveChildren(int position, int count);
    PGTATrackItem *GetChild(int row) const;
    int ChildCount() const;
    int ColumnCount() const;
    QVariant GetData(int column) const;
    bool SetData(int column, const QVariant &value);
    int GetRow() const;
    int GetChildRow(PGTATrackItem *child) const;
    PGTATrackItem *GetParent() const;
    bool IsGroup() const;
    void SetIsGroup(bool isGroup);
    QUuid GetUuid() const;

private:
    QList<PGTATrackItem*> m_childItems;
    QVector<QVariant> m_itemData;
    QUuid m_uuid;
    PGTATrackItem *m_parent;
    bool m_isGroup;
};
