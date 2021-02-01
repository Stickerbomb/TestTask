#include "mainwindow.h"
#include "treeitem.h"
#include "treemodel.h"
#include "typefile.h"

#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QtDebug>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
	setupUi(this);

	connect(exitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
	connect(insertColumnAction, &QAction::triggered, this, &MainWindow::insertColumn);
	connect(insertRowAction, &QAction::triggered, this, &MainWindow::insertRow);
	connect(removeColumnAction, &QAction::triggered, this, &MainWindow::removeColumn);
	connect(removeRowAction, &QAction::triggered, this, &MainWindow::removeRow);
	connect(insertChildAction, &QAction::triggered, this, &MainWindow::insertChild);
	connect(actionOpen_file, &QAction::triggered, this, &MainWindow::on_open_file_clicked);
	connect(actionSave_to_file, &QAction::triggered, this, &MainWindow::on_save_file_clicked);
    connect(actionCalculate_Hash_of_file, &QAction::triggered, this, &MainWindow::fileChecksum);

	xmlParser = std::make_unique<XmlParser>();
}

void MainWindow::insertChild()
{
	const QModelIndex index = view->selectionModel()->currentIndex();
	QAbstractItemModel *model = view->model();
	auto rowInsert = model->rowCount(index);
	if (!model->insertRow(rowInsert, index))
		return;

	const QModelIndex child = model->index(rowInsert, 0, index);
	model->setData(child, QVariant(tr("None")), Qt::EditRole);

	view->selectionModel()->setCurrentIndex(model->index(rowInsert, 0, index), QItemSelectionModel::ClearAndSelect);
}

void MainWindow::insertColumn()
{
	QAbstractItemModel *model = view->model();
	int column =view->selectionModel()->currentIndex().column();

	// Insert a column in the parent item.
	if (model->insertColumn(column + 1)) {
		model->setHeaderData(column + 1, Qt::Horizontal, QVariant(""), Qt::EditRole);
	}
		
}

void MainWindow::insertRow()
{
	const QModelIndex index = view->selectionModel()->currentIndex();
	QAbstractItemModel *model = view->model();

	const auto rowInsert = index.row() + 1;
	if (!model->insertRow(rowInsert, index.parent()))
		return;

	const auto indexInsert = model->index(index.row() + 1, 0, index.parent());
	model->setData(indexInsert, QVariant(tr("None")), Qt::EditRole);
	view->selectionModel()->setCurrentIndex(indexInsert, QItemSelectionModel::ClearAndSelect);
}

void MainWindow::removeColumn()
{
	QAbstractItemModel *model = view->model();
	const int column = view->selectionModel()->currentIndex().column();

	model->removeColumn(column);
}

void MainWindow::removeRow()
{
	const QModelIndex index = view->selectionModel()->currentIndex();
	QAbstractItemModel *model = view->model();
	model->removeRow(index.row(), index.parent());
}

void MainWindow::on_open_file_clicked()
{
	QString selectedFilter;

	QString filename = QFileDialog::getOpenFileName(this, tr("Open"), ".", TypeFile::filtersList(), &selectedFilter);

	if (filename != "") {
		const TypeFile typeFile{TypeFile::fromString(selectedFilter)};
		QFile file(filename);
		if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
			TreeModel *model = new TreeModel();
			model->setupModelData(xmlParser->read(file.readAll(), typeFile));
			view->setModel(model);
			for (int column = 0; column < model->columnCount(); ++column)
				view->setColumnWidth(column,view->width() / model->columnCount());
		}
		file.close();
	}
}

void MainWindow::on_save_file_clicked() {

	QString file_name;
	QString selectedFilter;
	QString filename = QFileDialog::getSaveFileName(this, tr("Save"), ".", TypeFile::filtersList(), &selectedFilter);

	if (filename != "") {
		const TypeFile typeFile{TypeFile::fromString(selectedFilter)};
		QFile file(filename);
		if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
			file.write(xmlParser->write(static_cast<TreeModel *>(view->model())->getRootItem(), typeFile));
			file.close();
		} else {
			qDebug() << "Failed to write file";
		}
	}
}

void MainWindow::fileChecksum()
{
    QString selectedFilter;

    QString filename = QFileDialog::getOpenFileName(this, tr("Open"), ".", TypeFile::filtersList(), &selectedFilter);

    QFile f(filename);
    if (f.open(QFile::ReadOnly)) {
        QCryptographicHash hash(QCryptographicHash::Algorithm::Md5);
        if (hash.addData(&f)) {
            QMessageBox msgBox;
            msgBox.setText("Hash by Md5 alhorythm");
            msgBox.setInformativeText(hash.result().toHex());
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setDefaultButton(QMessageBox::Ok);
            qDebug() << hash.result().toHex();
            int ret = msgBox.exec();
        }
    }
}
