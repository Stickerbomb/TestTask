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
    fileMenu->addAction(tr("&Save to XML"), this, &MainWindow::on_save_file_clicked, QKeySequence::Save);
    fileMenu->addAction(tr("&Save to Json"), this, &MainWindow::on_Save_to_json_Button_clicked);
    fileMenu->addAction(tr("E&xit"), this, &QWidget::close, QKeySequence::Quit);
    xmlParser = new XmlParser();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_open_file_clicked()
{
       /* Вызываем диалог выбора файла для чтения */
    ui->treeWidget->clear();
    QString selectedFilter;

    QString filename = QFileDialog::getOpenFileName(this, tr("Open"), ".",
                                                      TypeFile::filtersList(), &selectedFilter);
    if(filename != ""){
        TypeFile *tpFile = new TypeFile(TypeFile::fromString(selectedFilter));//Get type of file
        //load the xml file
        QFile file(filename);
        if(file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
           ui->treeWidget->insertTopLevelItem(0, xmlParser->read(file.readAll(),*tpFile));
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
        TypeFile *tpFile = new TypeFile(TypeFile::fromString(selectedFilter));//Get type of file
        //load the xml file
        QFile file(filename);
        if(file.open(QIODevice::WriteOnly | QIODevice::Text)){
            QTextStream stream(&file);
            stream << xmlParser->writeToFile(ui->treeWidget->itemAt(0,0),*tpFile);
            file.close();
        }
        else{
            qDebug() << "Failed to write file";
        }
    }

}


void MainWindow::on_Save_to_json_Button_clicked()
{


    QString file_name;
    QString filename = QFileDialog::getSaveFileName(this,
                                                tr("Open JSON"), ".",
                                                tr("Json files (*.json)"));
    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "Failed to write file";
    }
    file.open(QFile::WriteOnly | QFile::Text | QFile::Truncate);

    file.close();
}



void MainWindow::on_addButton_clicked()
{
    QTreeWidgetItem *item = new QTreeWidgetItem();
    ui->treeWidget->currentItem()->addChild(item);
}
