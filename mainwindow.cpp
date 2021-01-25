#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QDomElement>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(tr("&Open"), this, &MainWindow::on_open_file_clicked, QKeySequence::Open);
    fileMenu->addAction(tr("&Save"), this, &MainWindow::on_save_file_clicked, QKeySequence::Save);
    fileMenu->addAction(tr("E&xit"), this, &QWidget::close, QKeySequence::Quit);
    xmlParser = std::make_unique<XmlParser>();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_open_file_clicked()
{
       /* Вызываем диалог выбора файла для чтения */
    QString selectedFilter;

    QString filename = QFileDialog::getOpenFileName(this, tr("Open"), ".",
                                                      TypeFile::filtersList(), &selectedFilter);
    if(filename != ""){
        const TypeFile typeFile{TypeFile::fromString(selectedFilter)};
        //load the xml file
        QFile file(filename);
        if(file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            ui->treeWidget->clear();
            qDebug() << "Start";
            ui->treeWidget->insertTopLevelItem(0, xmlParser->read(file.readAll(),typeFile));
        }
        file.close();
        ui->treeWidget->model();
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
            file.write(xmlParser->write(ui->treeWidget->itemAt(0,0),typeFile));
            file.close();
        }
        else{
            qDebug() << "Failed to write file";
        }
    }

}


void MainWindow::on_addButton_clicked()
{
    QTreeWidgetItem *item = new QTreeWidgetItem();
    ui->treeWidget->currentItem()->addChild(item);
}
