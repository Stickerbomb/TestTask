#include "treeitem.h"

TreeItem::TreeItem(const QVector<QVariant> &data, TreeItem *parent)
	: itemData(data)
	, parentItem(parent)
{
}

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

QVector<TreeItem*> TreeItem::childs() const
{
	return childItems; 
}

int TreeItem::childNumber() const
{
	if (parentItem)
		return parentItem->childItems.indexOf(const_cast<TreeItem *>(this));
	
	return 0;
}

int TreeItem::columnCount() const
{
	return itemData.size();
}

QVariant TreeItem::data(int column) const 
{
	if (column < 0 || column >= itemData.size())
		return QVariant();
	
	return itemData.at(column);
}

bool TreeItem::insertChildren(int position, int count, int columns)
{
	if (position < 0 || position > childItems.size())
		return false;

	for (int row = 0; row < count; ++row) {
		TreeItem *item = new TreeItem(QVector<QVariant>{columns}, this);
		childItems.insert(position, item);
	}

	return true;
}

bool TreeItem::insertColumns(int position, int columns)
{
	if (position < 0 || position > itemData.size())
		return false;

	for (int col = 0; col < columns; ++col) {
		itemData.insert(position,QVariant());
	}

	for (TreeItem *child : qAsConst(childItems)) {
		child->insertColumns(position,columns);
	}
	
	return true;
}

bool TreeItem::addChild(TreeItem *item)
{
	if (item) {
		childItems.append(item);
	} else {
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

bool TreeItem::removeColumns(int position, int columns)
{
	if (position < 0 || position + columns > itemData.size())
		return false;
	
	for (int column = 0; column < columns; ++column)
		itemData.remove(position);
	
	for (TreeItem *child : qAsConst(childItems))
		child->removeColumns(position, columns);
	
	return true;
}

bool TreeItem::setData(int column,const QVariant &value) {
	
	if (column < 0 || column >= itemData.size())
		return false;
		
	itemData[column] = value;
	return true;
}
