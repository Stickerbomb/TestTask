#include "treemodel.h"
#include "treeitem.h"

QVariant TreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
		return rootItem->data(section);

	return QVariant();
}

QVariant TreeModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (role != Qt::DisplayRole && role != Qt::EditRole)
		return QVariant();

	return getItem(index)->data(index.column());
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
	if (!index.isValid())
		return Qt::NoItemFlags;

	return Qt::ItemIsEditable | QAbstractItemModel::flags(index);
}

TreeItem *TreeModel::getItem(const QModelIndex &index) const
{
	if (index.isValid()) {
		TreeItem *item = static_cast<TreeItem *>(index.internalPointer());
		if (item)
			return item;
	}
	return rootItem;
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &index) const
{
	if (index.isValid() && index.column() != 0)
		return QModelIndex();

	TreeItem *item = getItem(index);
	if (!item)
		return QModelIndex();

	TreeItem *childItem = item->child(row);
	if (childItem)
		return createIndex(row, column, childItem);
	
	return QModelIndex();
}

bool TreeModel::insertColumns(int position, int columns, const QModelIndex &index)
{
	beginInsertColumns(index, position, position + columns - 1);
	const bool success = rootItem->insertColumns(position, columns);
	endInsertColumns();
	
	return success;
}

bool TreeModel::removeColumns(int position, int columns, const QModelIndex &index)
{
	beginRemoveColumns(index, position, position + columns - 1);
	const bool success = rootItem->removeColumns(position, columns);
	endRemoveColumns();
	
	if (rootItem->columnCount() == 0)
		removeRows(0, rowCount());
	
	return success;
}

bool TreeModel::insertRows(int position, int rows, const QModelIndex &index)
{
	TreeItem *item = getItem(index);
	if (!item)
		return false;

	beginInsertRows(index, position, position + rows - 1);
	const bool success = item->insertChildren(position, rows,rootItem->columnCount());
	endInsertRows();

	return success;
}

QModelIndex TreeModel::parent(const QModelIndex &index) const
{
	if (!index.isValid())
		return QModelIndex();

	TreeItem *childItem = getItem(index);
	TreeItem *parentItem = childItem ? childItem->parent() : nullptr;

	if (parentItem == rootItem || !parentItem)
		return QModelIndex();

	return createIndex(parentItem->childNumber(), 0, parentItem);
}

bool TreeModel::removeRows(int position, int rows, const QModelIndex &parent)
{
	TreeItem *parentItem = getItem(parent);
	if (!parentItem)
		return false;

	beginRemoveRows(parent, position, position + rows - 1);
	const bool success = parentItem->removeChildren(position, rows);
	endRemoveRows();

	return success;
}

int TreeModel::rowCount(const QModelIndex &index) const
{
	const TreeItem *item = getItem(index);
	return item ? item->childCount() : 0;
}

int TreeModel::columnCount(const QModelIndex &index) const
{
	Q_UNUSED(index);
	return rootItem ? rootItem->columnCount() : 0;
}

bool TreeModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
	if (role != Qt::EditRole || orientation != Qt::Horizontal)
		return false;

	const bool result = rootItem->setData(section, value);

	if (result)
		emit headerDataChanged(orientation, section, section);

	return result;
}

bool TreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (role != Qt::EditRole)
		return false;

	TreeItem *item = getItem(index);
	bool result = item ? item->setData(index.column(),value) : false;

	if (result)
		emit dataChanged(index, index, {Qt::DisplayRole, Qt::EditRole});

	return result;
}

void TreeModel::setupModelData(TreeItem *_rootItem)
{
	rootItem = _rootItem; 
}

TreeItem *TreeModel::getRootItem() const
{
	return rootItem;
}
