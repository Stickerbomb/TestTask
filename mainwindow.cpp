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


    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(tr("&Open XML..."), this, &MainWindow::on_choose_file_button_clicked, QKeySequence::Open);
    fileMenu->addAction(tr("&Save to XML"), this, &MainWindow::on_choose_file_button_2_clicked, QKeySequence::Save);
    fileMenu->addAction(tr("&Open Json..."), this, &MainWindow::on_Load_from_json_Button_clicked);
    fileMenu->addAction(tr("&Save to Json"), this, &MainWindow::on_Save_to_json_Button_clicked);
    fileMenu->addAction(tr("E&xit"), this, &QWidget::close, QKeySequence::Quit);
    xmlParser = new XmlParser();
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
//    model->setHorizontalHeaderItem(1, new QStandardItem(QString("Attributes")));
//    model->setHorizontalHeaderItem(2, new QStandardItem(QString("Text")));
}

void MainWindow::read(QString Filename)
{
    if(model) delete model;
    model = new QStandardItemModel(0,1,this);
    ui->treeView->setModel(model);

    //load the xml file
    QFile file(Filename);
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        document.setContent(&file);
        file.close();
    }
    QStandardItem *root = new QStandardItem(document.firstChild().nodeName());
    model->appendRow(root);

    //get the xml root element
    QDomNode xmlroot = document.documentElement();
    traverseShow(xmlroot,root);


}

void MainWindow::traverseShow(const QDomNode &_elem, QStandardItem *_Model)
{
      const auto domNodeList = _elem.childNodes();
      for (int i = 0; i < domNodeList.length(); ++i) {
        const auto domNode = domNodeList.at(i);
        QDomElement domElement = domNode.toElement();
        const QDomNamedNodeMap attributeMap = domNode.attributes();

        QStringList attributes;
        for (int i = 0; i < attributeMap.count(); ++i) {
          QDomNode attribute = attributeMap.item(i);
          attributes << attribute.nodeName() + "=\"" + attribute.nodeValue() + '"';
        }
        // QStringList to QList<QStandartItem*> for appending to columns
        QString atr;
        for (const auto &i : attributes) {
            atr += i + " ";
        }

        QStandardItem *node = new QStandardItem(domElement.nodeName());
        if(domNode.hasAttributes()){
            QStandardItem *atr_item = new QStandardItem(atr);

            node->appendRow(atr_item);
        }
        if ((!domNode.hasChildNodes() || !domNode.firstChild().isElement()) && domElement.text() != "") {
            node->appendRow(new QStandardItem(domElement.text()));
        }
        _Model->appendRow(node);
        if (domNode.hasChildNodes() && domNode.firstChild().isElement()) {
            traverseShow(domNode, node);

          }
      }
        }



void MainWindow::writeXML(QStandardItem *item, QDomNode &dom_root)
{

    for(int i = 0; i< item->rowCount(); i++){
        if(item->child(i,0)->hasChildren()){
            QDomText newNodeText;
            if(!item->child(i,0)->child(1,0)->hasChildren()){
                    newNodeText = document.createTextNode(item->child(i,0)->child(1,0)->text());
            }
            QDomElement domelem = document.createElement(item->child(i,0)->text());
            domelem.appendChild(newNodeText);
            QStringList listArguments = item->child(i,0)->child(0,0)->text().split(' ');
            listArguments.removeLast();
            foreach(QString attr, listArguments){
                QStringList param = attr.split('=');
                param.first().remove('\"');
                param.last().remove('\"');
                domelem.setAttribute(param.first(),param.last());
            }
            dom_root.appendChild(domelem);

//            //Debug section
//            const QDomNamedNodeMap attributeMap = domelem.attributes();
//            QStringList attributes;
//            for (int j = 0; j < attributeMap.count(); ++j) {
//                QDomNode attribute = attributeMap.item(j);
//                attributes << attribute.nodeName() + "= "
//                              + attribute.nodeValue() + ' ';
//            }
//            // end Debug section
//            qDebug() <<attributes << "  ATRIBUTES  ";

            writeXML(item->child(i,0),domelem);
        }
    }


}


void MainWindow::on_choose_file_button_2_clicked()
{

    document.clear();
    if(model){
        QDomNode root = document.createElement((model->item(0,0)->text()));
        writeXML(model->item(0,0),root);
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

//Работа с Json
void MainWindow::on_Load_from_json_Button_clicked()
{
    if(model) delete model;
    model = new QStandardItemModel(0,3,this);
    ui->treeView->setModel(model);

    QString filename = QFileDialog::getOpenFileName(this,
                                              tr("Open JSON"), ".",
                                              tr("Json files (*.json)"));
    QString val;
    QFile file(filename);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    val = file.readAll();
    file.close();

    jsondocument = QJsonDocument::fromJson(val.toUtf8());
    QJsonObject jsonObject = jsondocument.object();


    document.clear();
    QStandardItem *root = new QStandardItem("Root");//toStdItem(sett2,"Root");
    QDomElement fromJsonRoot;
    fromJsonRoot = xmlParser->jsonToDom("Root", jsonObject);

    document.appendChild(fromJsonRoot);
    qDebug() << document.toString() << "DOM";
//    fromJsonRoot = document.documentElement();
    traverseShow(fromJsonRoot,root);
    model->appendRow(root);


}


void MainWindow::on_Save_to_json_Button_clicked()
{
    QJsonDocument JsonDocument = QJsonDocument();
    QJsonObject rootObject = JsonDocument.object();
    xmlParser->writeJson(model->item(0,0),rootObject);
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


