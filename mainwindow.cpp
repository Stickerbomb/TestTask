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
            QStandardItem *atr_item = new QStandardItem(atr);
            node->appendRow(atr_item);
            if (!domNode.hasChildNodes() || !domNode.firstChild().isElement()) {
              node->appendRow(new QStandardItem(domElement.text()));
            }
            _Model->appendRow(node);
            if (domNode.hasChildNodes() && domNode.firstChild().isElement()) {
              traverseShow(domNode, node);
            }
          }
        }



QStandardItem* MainWindow::toStdItem(const QJsonArray &jarray, QString parent)
{
//    qDebug() << jarray << "|>  ARRAY  <|";
    QStandardItem *result = new QStandardItem(parent);
    QStandardItem *item;

    foreach(const QJsonValue &value, jarray){

        if(value.isDouble()) {
            QString str = QString::fromStdString(std::to_string(value.toDouble()));
            item = new QStandardItem(str);
        }

        if(value.isString()) {
            QString str = value.toString();
            item = new QStandardItem(str);
        }
        if(value.isBool()) {
            QString str = value.toBool() ? "true" : "false" ;
            item = new QStandardItem(str);
        }
        if(value.isObject()){
            item = toStdItem(value.toObject(), "[" + parent + "]");

        }
        if(value.isArray()){
            item = toStdItem(value.toArray(), "Array");
        }
        result->appendRow(item);

    }
    return result;
}

QStandardItem *MainWindow::toStdItem(const QJsonObject &jo, QString parent)
{
//    qDebug() << jo << "|  OBJECT  |";
    QStandardItem *item;
    QStandardItem *result = new QStandardItem(parent);
    foreach(const QString & str, jo.keys()){
        item = new QStandardItem(str);
        QStandardItem *item_value;
        QString str_value;

        // Надо проврить тип данных, придется громоздко через if/else

        if(jo.value(str).isDouble()){
            str_value = QString::fromStdString(std::to_string(jo.value(str).toDouble()));
//            qDebug() << str_value << " 2 ";
            item_value = new QStandardItem(str_value);
            item->appendRow(item_value);
        }
        else{
            if(jo.value(str).isString()){
                str_value = jo.value(str).toString();
//                qDebug() << str_value << " 2 ";
                item_value = new QStandardItem(str_value);
                item->appendRow(item_value);
            }
            else{

                if(jo.value(str).isBool()){
                     str_value = jo.value(str).toBool() ? "true" : "false";
//                     qDebug() << str_value << " 2 ";
                     item_value = new QStandardItem(str_value);
                     item->appendRow(item_value);
                }
                else{

                    if(jo.value(str).isArray()){
                        item = toStdItem(jo.value(str).toArray(), str);
                        item_value = new QStandardItem();
                    }

                    if(jo.value(str).isObject()){
                        item = toStdItem(jo.value(str).toObject(), str);
                        item_value = new QStandardItem();
                    }
                }
            }
        }

        result->appendRow(item);


        qDebug() << item->text() << item_value->text() << " 11 ";
    }
    return result;
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

void MainWindow::writeJson(QStandardItem *item, QJsonObject &json_root)
{
    for(int i = 0; i< item->rowCount(); i++){
        QStandardItem *currentItem = item->child(i,0);

        if(currentItem->hasChildren() && !currentItem->child(0,0)->hasChildren() && currentItem->rowCount()==1){    //we find a simple node
            json_root.insert(currentItem->text(),currentItem->child(0,0)->text());
        }
        if(currentItem->child(0,0)->text().at(0)=="["){     // we find an array
            QJsonArray  *jArray = new QJsonArray();
            writeJson(currentItem,*jArray);
            json_root.insert(currentItem->child(0,0)->text(), *jArray);
        }
        if(currentItem->hasChildren() && currentItem->child(0,0)->hasChildren()){   //we find an object
            QJsonObject *jsonObject = new QJsonObject();
            writeJson(currentItem,*jsonObject);
            json_root.insert(currentItem->child(0,0)->text(),*jsonObject);
        }
    }

}


void MainWindow::writeJson(QStandardItem *item, QJsonArray &json_root)
{
    for(int i = 0; i< item->rowCount(); i++){
        QStandardItem *currentItem = item->child(i,0);

        if(currentItem->hasChildren() && !currentItem->child(0,0)->hasChildren() && currentItem->rowCount()==1){ // simple node
            QJsonValue *value = new QJsonValue(currentItem->child(0,0)->text());
            json_root.append(*value);
        }
        if(currentItem->child(0,0)->text().at(0)=="["){
            QJsonArray  *jArray = new QJsonArray();
            writeJson(currentItem,*jArray);
            json_root.append(*jArray);
        }
        if(currentItem->hasChildren() && currentItem->child(0,0)->hasChildren()){   //we find an object
            QJsonObject *jsonObject = new QJsonObject();
            writeJson(currentItem,*jsonObject);
            json_root.append(*jsonObject);
        }
    }

}

QJsonValue* MainWindow::stringToJson(QString str)
{
    QJsonValue *result = new QJsonValue();
    if(str== "true" || str == "false"){
        if(str=="true"){
            result= new QJsonValue(true);
        }
        else{
            result= new QJsonValue(false);
        }
        return result;
    }
    bool ok;
    // Double dec = str.toDouble(&ok, 10);
}

QDomElement MainWindow::jsonToDom(QJsonObject jsondoc, const QString name) {
    QDomElement result = document.createElement(name);
    for (const auto &key : jsondoc.keys()) {
        result.appendChild(jsonToDom(key, jsondoc.value(key)));
    }
    qDebug() << result.nodeName() << "added OBJECT";
    return result;
}

QDomElement MainWindow::jsonToDom(const QString &key, const QJsonValue &value) {
    QDomElement node = document.createElement(key);
    int i=0;
    switch (value.type()) {
        case QJsonValue::Type::Bool:
            qDebug() << key << ": [bool] " << value.toBool();
            node.appendChild(document.createElement( QString::fromStdString( std::to_string( value.toBool()))));
            break;

        case QJsonValue::Type::Double:
            qDebug() << key << ": [double] " << value.toDouble();
            node.appendChild( document.createElement( QString::fromStdString( std::to_string(value.toDouble()))));
            break;

        case QJsonValue::Type::String:
            qDebug() << key << ": [string] " << value.toString();
            node.appendChild( document.createElement( value.toString()));
            break;

        case QJsonValue::Type::Null:
            qDebug() << key << ": [null] ";
            node.appendChild( document.createElement( "[null]"));
            break;

        case QJsonValue::Type::Array:
            qDebug() << key << ": [array] ";
            for (const auto &o : value.toArray()) {
                node.appendChild(jsonToDom(key + QString::fromStdString( std::to_string(i)), o));
                i++;
            }
            break;

        case QJsonValue::Type::Object:
            qDebug() << key << ": [object] ";

            node = jsonToDom(value.toObject(),key);
            break;
    }
    if(node.isNull()){
        qDebug() << " NULL ";
    }
    return node;
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
    fromJsonRoot = jsonToDom("Root", jsonObject);

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
    writeJson(model->item(0,0),rootObject);
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


