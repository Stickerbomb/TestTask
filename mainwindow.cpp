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

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_choose_file_button_clicked()
{
       /* Вызываем диалог выбора файла для чтения */
    QString filename = QFileDialog::getOpenFileName(this,
                                              tr("Open Xml"), ".",
                                              tr("Xml files (*.xml)"));

    if(filename != ""){
               Read(filename);
    }

    model->setHorizontalHeaderItem(0, new QStandardItem(QString("Name")));
//    model->setHorizontalHeaderItem(1, new QStandardItem(QString("Attributes")));
//    model->setHorizontalHeaderItem(2, new QStandardItem(QString("Text")));
}

void MainWindow::Read(QString Filename)
{

//    QDomDocument document;
    delete model;
    QStandardItem *root = new QStandardItem("Root");
    model = new QStandardItemModel(0,1,this);
    model->appendRow(root);
    ui->treeView->setModel(model);

    //load the xml file
    QFile file(Filename);
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        document.setContent(&file);
        file.close();
    }

    //get the xml root element
    QDomNode xmlroot = document.documentElement();
    Show(xmlroot,root);
    //read the books
    QDomNode nodeList = xmlroot;
//     const QModelIndex a = *new QModelIndex();
//    for (int i =0; i< model->rowCount() ; i++) {
//        qDebug() << model->data( a,1);
//    }

}

void MainWindow::Write(QString root)
{

}

void MainWindow::Show(const QDomNode &_elem, QStandardItem *_Model)
{
    QDomNode domNode = _elem.firstChild();
    while(!domNode.isNull())
    {
        if(domNode.isElement())
        {
            QDomElement domElement = domNode.toElement();
            QStandardItem *nodeText = new QStandardItem(domElement.nodeName());
//            qDebug() << domElement.nodeName() ;


            const QDomNamedNodeMap attributeMap = domNode.attributes();
            QStringList attributes;
            for (int i = 0; i < attributeMap.count(); ++i) {
                QDomNode attribute = attributeMap.item(i);
                attributes << attribute.nodeName() + "=\""
                              + attribute.nodeValue() + '"';
            }

            QString atr;
            // QStringList to QList<QStandartItem> for appending to columns
            QList<QStandardItem*> *temp = new QList<QStandardItem*>;
            for ( const auto& i : attributes  ){
//                qDebug() << i;
                atr += i;
                QStandardItem *cratch = new QStandardItem(i);
                temp->append(cratch);
            }
//             QStandardItem *cratch = new QStandardItem(temp);
//            temp->append(cratch);
//           qDebug() << _Model->column();

//            for ( const auto& i : *temp  ){
//                qDebug() << " (" << i->text() << ") ";
//            }

            _Model->appendRow(nodeText);
            nodeText->appendColumn(*temp);
            if(domNode.hasChildNodes()){
                Show(domNode, nodeText);
            }
        }
        domNode = domNode.nextSibling();
    }
}

void MainWindow::on_choose_file_button_2_clicked()
{
//    QDomDocument document;
//    QDomElement xmlroot = document.createElement("Root");
//       document.appendChild(xmlroot);






    //Сохранить в файл
    QString file_name;
    QString filename = QFileDialog::getOpenFileName(this,
                                              tr("Open Xml"), ".",
                                              tr("Xml files (*.xml)"));

    if(filename != ""){
               Read(filename);
    }
    QFile file(filename);
       if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
       {
           qDebug() << "Failed to write file";
       }

       QTextStream stream(&file);
       stream << document.toString();
       file.close();
}
