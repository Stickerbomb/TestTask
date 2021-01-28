#ifndef TREEITEM_H
#define TREEITEM_H

#include <QVariant>
#include <QVector>


class TreeItem
{
public:
    explicit TreeItem(const QVariant &data, TreeItem *parent = nullptr);
    ~TreeItem();

    TreeItem *child(int number);
    int childCount() const;
    int columnCount() const;
    QVariant data() const;
    bool insertChildren(int position, int count);
    bool addChild(TreeItem *item);
    TreeItem *parent();
    bool removeChildren(int position, int count);
    int childNumber() const;
    bool setData(const QVariant &value);

private:
    QVector<TreeItem*> childItems;
    QVariant itemData;
    TreeItem *parentItem;
};

#endif // TREEITEM_H
