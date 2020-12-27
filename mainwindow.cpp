#include "mainwindow.h"
#include "classes.h"
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
               read(filename);
    }

    model->setHorizontalHeaderItem(0, new QStandardItem(QString("Name")));
    model->setHorizontalHeaderItem(1, new QStandardItem(QString("Attributes")));
    model->setHorizontalHeaderItem(2, new QStandardItem(QString("Text")));
}

void MainWindow::read(QString Filename)
{
    if(model) delete model;
    QStandardItem *root = new QStandardItem("Root");
    model = new QStandardItemModel(0,3,this);
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
    traverseShow(xmlroot,root);


}

void MainWindow::traverseShow(const QDomNode &_elem, QStandardItem *_Model)
{
    QDomNode domNode = _elem.firstChild();
    while(!domNode.isNull())
    {
        if(domNode.isElement())
        {
            QDomElement domElement = domNode.toElement();


            const QDomNamedNodeMap attributeMap = domNode.attributes();
            QStringList attributes;
            for (int i = 0; i < attributeMap.count(); ++i) {
                QDomNode attribute = attributeMap.item(i);
                attributes << attribute.nodeName() + "=\""
                              + attribute.nodeValue() + '"';
            }

            // QStringList to QList<QStandartItem*> for appending to columns
            QString atr;
            for ( const auto& i : attributes  ){
                atr += i + " ";
            }

            QStandardItem *node = new QStandardItem(domElement.nodeName());
            QStandardItem *atr_item = new QStandardItem(atr);
            QStandardItem *text_item = new QStandardItem(domElement.text());
            qDebug() << domElement.nodeName() << atr << domElement.text();

            node->setChild(0, 1, atr_item);
            node->setChild(0, 2, text_item);
            _Model->appendRow(node);
            if(domNode.hasChildNodes()){
                traverseShow(domNode, node);
            }
        }
        domNode = domNode.nextSibling();
    }
}

void MainWindow::write(QStandardItem *item, QDomDocument &dom_root)
{
    QDomElement tem_node =dom_root.createElement(model->item(0,0)->text());
    qDebug() << model->item(0,0)->text();
//    tem_node.setAttribute();
}

void MainWindow::on_choose_file_button_2_clicked()
{
//    QDomDocument dom_doc;
//    QDomElement xmlroot = document.createElement("Root");
//    dom_doc.appendChild(xmlroot);
//    write(model->item(0,0),dom_doc);



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

//Работа с Json
void MainWindow::on_Load_from_json_Button_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this,
                                              tr("Open JSON"), ".",
                                              tr("Json files (*.json)"));
    QString val;
    QFile file(filename);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    val = file.readAll();
    file.close();
    jsondocument = QJsonDocument::fromJson(val.toUtf8());
    QJsonObject sett2 = jsondocument.object();
    serializer.fromJson(val.toUtf8());
    document = serializer.appendXmlHat(serializer.toXml(),"UTF-8");
    QDomNode xmlroot = document.documentElement();
    qDebug() << xmlroot.nodeName();

    TestXml src;
    QDomDocument node = QSerializer::appendXmlHat(src.toXml(), "UTF-8");
        qDebug()<<QSerializer::toByteArray(node).constData();

    if(model) delete model;
    QStandardItem *root = new QStandardItem("Root");
    model = new QStandardItemModel(0,3,this);
    model->appendRow(root);
    ui->treeView->setModel(model);
    traverseShow(xmlroot,root);
}



void MainWindow::on_Save_to_json_Button_clicked()
{
    QJsonDocument JsonDocument = QJsonDocument(serializer.toJson());
    QJsonObject RootObject = JsonDocument.object();
    JsonDocument.setObject(RootObject); // set to json document

    QString file_name;
    QString filename = QFileDialog::getSaveFileName(this,
                                              tr("Open Xml"), ".",
                                              tr("Xml files (*.xml)"));

    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "Failed to write file";
    }
    file.open(QFile::WriteOnly | QFile::Text | QFile::Truncate);
    file.write(JsonDocument.toJson());
    file.close();
}
