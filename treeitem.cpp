#include "treeitem.h"

TreeItem::TreeItem(const QVariant &data, TreeItem *parent)
    : itemData(data),
      parentItem(parent)
{}

TreeItem::~TreeItem()
{
    qDeleteAll(childItems);
}

TreeItem *TreeItem::child(int number)
{
    if (number < 0 || number >= childItems.size())
        return nullptr;
    return childItems.at(number);
}

int TreeItem::childCount() const
{
    return childItems.count();
}

int TreeItem::childNumber() const
{
    if (parentItem)
        return parentItem->childItems.indexOf(const_cast<TreeItem*>(this));
    return 0;
}

int TreeItem::columnCount() const
{
    return 1;
}

QVariant TreeItem::data() const
{
    return itemData;
}

bool TreeItem::insertChildren(int position, int count)
{
    if (position < 0 || position > childItems.size())
        return false;

    for (int row = 0; row < count; ++row) {
        QVariant data{};
        TreeItem *item = new TreeItem(data, this);
        childItems.insert(position, item);
    }

    return true;
}

bool TreeItem::addChild(TreeItem *item)
{
    if(item){
        childItems.append(item);
    }
    else{
        return false;
    }
    return true;
}

TreeItem *TreeItem::parent()
{
    return parentItem;
}

bool TreeItem::removeChildren(int position, int count)
{
    if (position < 0 || position + count > childItems.size())
        return false;

    for (int row = 0; row < count; ++row)
        delete childItems.takeAt(position);

    return true;
}

bool TreeItem::setData(const QVariant &value)
{
    itemData = value;
    return true;
}
