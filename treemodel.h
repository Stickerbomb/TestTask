#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>

class TreeItem;

class TreeModel : public QAbstractItemModel {
		Q_OBJECT

public:
	using QAbstractItemModel::QAbstractItemModel;

	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
	QVariant data(const QModelIndex &index, int role) const override;

	QModelIndex index(int row, int column, const QModelIndex &index = QModelIndex()) const override;
	QModelIndex parent(const QModelIndex &index) const override;

	int rowCount(const QModelIndex &index = QModelIndex()) const override;
	int columnCount(const QModelIndex &index = QModelIndex()) const override;

	Qt::ItemFlags flags(const QModelIndex &index) const override;
	bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role) override;
	bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
	
	bool insertColumns(int position, int columns, const QModelIndex &parent = QModelIndex()) override;
	bool removeColumns(int position, int columns, const QModelIndex &parent = QModelIndex()) override;

	bool insertRows(int position, int rows, const QModelIndex &parent = QModelIndex()) override;
	bool removeRows(int position, int rows, const QModelIndex &parent = QModelIndex()) override;

	void setupModelData(TreeItem *_rootItem);
	
	TreeItem *getItem(const QModelIndex &index) const;
	TreeItem *getRootItem() const;

private:
		TreeItem *rootItem;
};

#endif // TREEMODEL_H
