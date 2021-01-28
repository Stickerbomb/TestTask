#include "mainwindow.h"
#include "treemodel.h"
#include "typefile.h"
#include "treeitem.h"

#include <QtDebug>
#include <QFile>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi(this);

    TreeModel *model = new TreeModel();

    view->setModel(model);
    connect(exitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
    connect(insertRowAction, &QAction::triggered, this, &MainWindow::insertRow);
    connect(removeRowAction, &QAction::triggered, this, &MainWindow::removeRow);
    connect(insertChildAction, &QAction::triggered, this, &MainWindow::insertChild);
    connect(actionOpen_file, &QAction::triggered, this, &MainWindow::on_open_file_clicked);
    connect(actionSave_to_file, &QAction::triggered, this, &MainWindow::on_save_file_clicked);

    xmlParser = std::make_unique<XmlParser>();
}

void MainWindow::insertChild()
{
    const QModelIndex index = view->selectionModel()->currentIndex();
    QAbstractItemModel *model = view->model();

    if (!model->insertRow(0, index))
        return;


    const QModelIndex child = model->index(model->rowCount(), 0, index);
    model->setData(child, QVariant(tr("[Empty node]")), Qt::EditRole);


    view->selectionModel()->setCurrentIndex(model->index(0, 0, index),
                                            QItemSelectionModel::ClearAndSelect);
}

void MainWindow::insertRow()
{
    const QModelIndex index = view->selectionModel()->currentIndex();
    QAbstractItemModel *model = view->model();

    if (!model->insertRow(index.row()+1, index.parent()))
        return;
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

    QString filename = QFileDialog::getOpenFileName(this, tr("Open"), ".",
                                                      TypeFile::filtersList(), &selectedFilter);
    if(filename != ""){
        const TypeFile typeFile{TypeFile::fromString(selectedFilter)};
        //load the xml file
        QFile file(filename);
        if(file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            TreeModel *model = new TreeModel();
            model->setupModelData(xmlParser->read(file.readAll(),typeFile));
            view->setModel(model);
            qDebug() << model->getRootItem()->data().toString();
        }
        file.close();
    }
}


void MainWindow::on_save_file_clicked()
{

    //Сохранить в файл
    QString file_name;
    QString selectedFilter;
    QString filename = QFileDialog::getSaveFileName(this,
                                              tr("Save"), ".",
                                                TypeFile::filtersList(), &selectedFilter);


    if(filename != ""){
        const TypeFile typeFile{TypeFile::fromString(selectedFilter)};
        //load the xml file
        QFile file(filename);
        if(file.open(QIODevice::WriteOnly | QIODevice::Text)){
            file.write(xmlParser->write(static_cast<TreeModel*>(view->model())->getRootItem(),typeFile));
            file.close();
        }
        else{
            qDebug() << "Failed to write file";
        }
    }

}
