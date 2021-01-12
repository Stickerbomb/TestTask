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
            node->appendRow(atr_item);
            node->appendRow(text_item);
            _Model->appendRow(node);
            if(domNode.hasChildNodes()){
                traverseShow(domNode, node);
            }
        }
        domNode = domNode.nextSibling();
    }
}

QDomNode MainWindow::toDomNode(const QStandardItem &parent)
{
    QDomNode result;
//    foreach(const QStandardItem &currentItem, parent.ch)
    return result;
}

QStandardItem* MainWindow::toStdItem(const QJsonArray &jarray, QString parent)
{
    qDebug() << jarray << "  1  ";
    QStandardItem *result = new QStandardItem(parent);
    QString str;
    QStandardItem *item;

    foreach(const QJsonValue &value, jarray){
        if(value.isDouble()) {
            str += QString::number(value.toInt()) +", ";
        }

        if(value.isString()) {
                str += "\"" + value.toString() +"\", ";
        }
        if(value.isBool()) {
                QString tem_str = value.toBool() ? "true" : "false" ;
                str += tem_str + ", ";
        }
        if(value.isObject()){
            QStandardItem *temp = toStdItem(value.toObject(),parent);
            result->appendRow(temp);
        }
    }
    if(str!=""){
        str = str.remove(str.length()-2,2);
    }
    item = new QStandardItem(str);
    result->appendRow(item);

    return result;
}

QStandardItem *MainWindow::toStdItem(const QJsonObject &jo, QString parent)
{
    QStandardItem *item;
    QStandardItem *result = new QStandardItem(parent);
    foreach(const QString & str, jo.keys()){
        item = new QStandardItem(str);
        QStandardItem *item_value;
        QString str_value;

        // Надо проврить тип данных, придется громоздко через if/else

        if(jo.value(str).isDouble()){
            str_value = QString::fromStdString(std::to_string(jo.value(str).toDouble()));
            qDebug() << str_value << " 2 ";
            item_value = new QStandardItem(str_value);
            item->appendRow(item_value);
        }
        else{
            if(jo.value(str).isString()){
                str_value = jo.value(str).toString();
                qDebug() << str_value << " 2 ";
                item_value = new QStandardItem(str_value);
                item->appendRow(item_value);
            }
            else{

                if(jo.value(str).isBool()){
                     str_value = jo.value(str).toBool() ? "true" : "false";
                     qDebug() << str_value << " 2 ";
                     item_value = new QStandardItem(str_value);
                     item->appendRow(item_value);
                }
                else{

                    if(jo.value(str).isArray()){
                        item = toStdItem(jo.value(str).toArray(), str);
                        item_value = new QStandardItem();
                        qDebug() << str << "ARRAY 2 ";
                    }

                    if(jo.value(str).isObject()){
                        item = toStdItem(jo.value(str).toObject(), str);
                        item_value = new QStandardItem();
                        qDebug() << str << "OBJECT 2 ";
                    }
                }
            }
        }

        result->appendRow(item);


        qDebug() << item->text() << item_value->text() << " 11 ";
    }
    return result;
}

void MainWindow::write(QStandardItem *item, QDomNode &dom_root)
{

    for(int i = 0; i< item->rowCount(); i++){
        if(item->child(i,0)->hasChildren()){
            QDomText newNodeText = document.createTextNode(item->child(i,0)->child(1,0)->text());
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

            //Debug section
            const QDomNamedNodeMap attributeMap = domelem.attributes();
            QStringList attributes;
            for (int j = 0; j < attributeMap.count(); ++j) {
                QDomNode attribute = attributeMap.item(j);
                attributes << attribute.nodeName() + "= "
                              + attribute.nodeValue() + ' ';
            }
            // end Debug section
            qDebug() <<attributes << "  ATRIBUTES  ";

            write(item->child(i,0),domelem);
        }
    }


}

void MainWindow::on_choose_file_button_2_clicked()
{
//    QDomDocument dom_doc;
//    QDomElement xmlroot = document.createElement("Root");
//    dom_doc.appendChild(xmlroot);
//    write(model->item(0,0),dom_doc);
    document.clear();
    if(model){
        QDomNode root = document.createElement((model->item(0,0)->text()));
        write(model->item(0,0),root);
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
    QJsonObject sett2 = jsondocument.object();

    QStandardItem *root = toStdItem(sett2,"Root");
    model->appendRow(root);


}


void MainWindow::on_Save_to_json_Button_clicked()
{
    QJsonDocument JsonDocument = QJsonDocument();
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
