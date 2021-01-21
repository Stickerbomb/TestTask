#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QDomElement>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
      model(nullptr)
{
    ui->setupUi(this);


    model = new QStandardItemModel(0,1,this);

    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(tr("&Open"), this, &MainWindow::on_open_file_clicked, QKeySequence::Open);
    fileMenu->addAction(tr("&Save to XML"), this, &MainWindow::on_choose_file_button_2_clicked, QKeySequence::Save);
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
    model->clear();
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
            qDebug()<< "Danone";
           ui->treeWidget->insertTopLevelItem(0, xmlParser->read(file.readAll(),*tpFile));
           qDebug()<< "Danone2";
        }
        file.close();
        ui->treeWidget->model();
    }
}


void MainWindow::on_choose_file_button_2_clicked()
{

    document.clear();
    if(model){
        QDomNode root = document.createElement((model->item(0,0)->text()));
        xmlParser->writeXML(ui->treeWidget->itemAt(0,0),root);
        document.appendChild(root);

    }

    //Сохранить в файл
    QString file_name;
    QString filename = QFileDialog::getSaveFileName(this,
                                              tr("Open Xml"), ".",
                                              tr("Xml files (*.xml)"));

    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "Failed to write file";
    }

    QTextStream stream(&file);
    stream << document.toString();
    file.close();
}


void MainWindow::on_Save_to_json_Button_clicked()
{
    QJsonDocument JsonDocument = QJsonDocument();
    QJsonObject rootObject = JsonDocument.object();

    JsonDocument.setObject(rootObject); // set to json document

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
    file.write(JsonDocument.toJson());
    file.close();
}



void MainWindow::on_addButton_clicked()
{

}
