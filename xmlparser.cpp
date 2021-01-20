#include "xmlparser.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QMainWindow>

XmlParser::XmlParser()
{

}

QStandardItem* XmlParser::read(const QByteArray byteArray,  TypeFile& type)
{
    QStandardItem *treeStdItem;
    QDomElement tree;
    document.clear();
    if(type.value() == TypeFile::Value::Json){


        QString jsonStr = byteArray;
        jsondocument = QJsonDocument::fromJson(jsonStr.toUtf8());
        QJsonObject jsonObject = jsondocument.object();
        tree = jsonToDom("Root", jsonObject);

        document.appendChild(tree);
    }
    else{
        if(type.value() == TypeFile::Value::Json){
            document.setContent(byteArray);
            tree = document.firstChild().toElement();
        }
        else {
            return new QStandardItem();
        }
    }
    treeStdItem = new QStandardItem(document.firstChild().nodeName());
    traverseShow(tree,treeStdItem);
    return treeStdItem;
}

void XmlParser::writeToFile(QString filename, QString type)
{
    QJsonDocument JsonDocument = QJsonDocument();
    QJsonObject rootObject = JsonDocument.object();

    //    XML TREE TO JSON HERE

    JsonDocument.setObject(rootObject);
    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "Failed to write file";
    }
    file.open(QFile::WriteOnly | QFile::Text | QFile::Truncate);
    file.write(JsonDocument.toJson());
    file.close();
}

void XmlParser::traverseShow(const QDomNode &_elem, QStandardItem *_Model)
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

void XmlParser::writeJson(QStandardItem *item, QJsonObject &json_root)
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

void XmlParser::writeJson(QStandardItem *item, QJsonArray &json_root)
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

QStandardItem* XmlParser::toStdItem(const QJsonArray &jarray, QString parent)
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

QStandardItem *XmlParser::toStdItem(const QJsonObject &jo, QString parent)
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

    }
    return result;
}

QJsonValue* XmlParser::stringToJson(QString str)
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

QDomElement XmlParser::jsonToDom(QJsonObject jsondoc, const QString name) {
    QDomElement result = document.createElement(name);
    for (const auto &key : jsondoc.keys()) {
        result.appendChild(jsonToDom(key, jsondoc.value(key)));
    }
    return result;
}

QDomElement XmlParser::jsonToDom(const QString &key, const QJsonValue &value) {
    QDomElement node = document.createElement(key);
    int i=0;
    switch (value.type()) {
        case QJsonValue::Type::Bool:
//            qDebug() << key << ": [bool] " << value.toBool();
            node.appendChild(document.createElement( QString::fromStdString( std::to_string( value.toBool()))));
            break;

        case QJsonValue::Type::Double:
//            qDebug() << key << ": [double] " << value.toDouble();
            node.appendChild( document.createElement( QString::fromStdString( std::to_string(value.toDouble()))));
            break;

        case QJsonValue::Type::String:
//            qDebug() << key << ": [string] " << value.toString();
            node.appendChild( document.createElement( value.toString()));
            break;

        case QJsonValue::Type::Null:
//            qDebug() << key << ": [null] ";
            node.appendChild( document.createElement( "[null]"));
            break;

        case QJsonValue::Type::Array:
//            qDebug() << key << ": [array] ";
            for (const auto &o : value.toArray()) {
                node.appendChild(jsonToDom(key + QString::fromStdString( std::to_string(i)), o));
                i++;
            }
            break;

        case QJsonValue::Type::Object:
//            qDebug() << key << ": [object] ";

            node = jsonToDom(value.toObject(),key);
            break;
    }
    return node;
}
