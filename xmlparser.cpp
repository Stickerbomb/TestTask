#include "xmlparser.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QMainWindow>

XmlParser::XmlParser()
{

}

QTreeWidgetItem* XmlParser::read(const QByteArray byteArray,  TypeFile& type)
{
    QTreeWidgetItem *treeStdItem;
    QDomElement tree;
    document.clear();
    switch(type.value()){
        case TypeFile::Value::Json:{

            qDebug() << "JSON";
            QString jsonStr = byteArray;
            jsondocument = QJsonDocument::fromJson(jsonStr.toUtf8());
            QJsonObject jsonObject = jsondocument.object();
            tree = jsonToXml("Root", jsonObject);
            document.appendChild(tree);
            break;
        }
        case TypeFile::Value::Xml:{
                qDebug() << "XML";
                document.setContent(byteArray);
                tree = document.firstChild().toElement();
                break;
        }

        case TypeFile::Value::Undefined:{
                qDebug() << "I DONT KNOW";
                return new QTreeWidgetItem();
                break;
        }
    }
    QStringList name;
    name.append(document.firstChild().nodeName());
    treeStdItem = new QTreeWidgetItem(name);
    xmlToTree(tree,treeStdItem);
    return treeStdItem;
}

void XmlParser::writeToFile(QString filename, TypeFile& type)
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

void XmlParser::xmlToTree(const QDomNode &_elem, QTreeWidgetItem *_Model)
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
        QStringList name;
        name.append(domElement.nodeName());
        QTreeWidgetItem *node = new QTreeWidgetItem(name);
        if(domNode.hasAttributes()){

            QTreeWidgetItem *atr_item = new QTreeWidgetItem(attributes);
            node->addChild(atr_item);
        }
        if ((!domNode.hasChildNodes() || !domNode.firstChild().isElement()) && domElement.text() != "") {
            QStringList text;
            text.append(domElement.text());
            node->addChild(new QTreeWidgetItem(text));
        }
        _Model->addChild(node);
        if (domNode.hasChildNodes() && domNode.firstChild().isElement()) {
            xmlToTree(domNode, node);
        }
    }
}

void XmlParser::writeXML(QTreeWidgetItem *item, QDomNode &dom_root)
{

    for(int i = 0; i< item->childCount(); i++){
        if(item->child(i)->childCount()>0){
            QDomText newNodeText;
            if(!item->child(i)->child(1)->childCount()>0){
                    newNodeText = document.createTextNode(item->child(i)->child(1)->text(0));
            }
            QDomElement domelem = document.createElement(item->child(i)->text(0));
            domelem.appendChild(newNodeText);
            QStringList listArguments = item->child(i)->child(0)->text(0).split(' ');
            listArguments.removeLast();
            foreach(QString attr, listArguments){
                QStringList param = attr.split('=');
                param.first().remove('\"');
                param.last().remove('\"');
                domelem.setAttribute(param.first(),param.last());
            }
            dom_root.appendChild(domelem);
/*
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
*/
            writeXML(item->child(i),domelem);
        }
    }


}

void XmlParser::writeJson(QTreeWidgetItem *item, QJsonObject &json_root)
{
    for(int i = 0; i< item->childCount(); i++){
        QTreeWidgetItem *currentItem = item->child(i);

        if(currentItem->childCount()==1 && currentItem->child(0)->childCount()>0){    //we find a simple node
            json_root.insert(currentItem->text(0),currentItem->child(0)->text(0));
        }
        if(currentItem->child(0)->text(0).at(0)=="["){     // we find an array
            QJsonArray  *jArray = new QJsonArray();
            writeJson(currentItem,*jArray);
            json_root.insert(currentItem->child(0)->text(0), *jArray);
        }
        if(currentItem->childCount()>0 && currentItem->child(0)->childCount()>0){   //we find an object
            QJsonObject *jsonObject = new QJsonObject();
            writeJson(currentItem,*jsonObject);
            json_root.insert(currentItem->child(0)->text(0 ),*jsonObject);
        }
    }

}

void XmlParser::writeJson(QTreeWidgetItem *item, QJsonArray &json_root)
{
    for(int i = 0; i< item->childCount(); i++){
        QTreeWidgetItem *currentItem = item->child(i);

        if(currentItem->childCount()==1 && currentItem->child(0)->childCount()>0){ // simple node
            QJsonValue *value = new QJsonValue(currentItem->child(0)->text(0));
            json_root.append(*value);
        }
        if(currentItem->child(0)->text(0).at(0)=="["){
            QJsonArray  *jArray = new QJsonArray();
            writeJson(currentItem,*jArray);
            json_root.append(*jArray);
        }
        if(currentItem->childCount()>0 && currentItem->child(0)->childCount()>0){   //we find an object
            QJsonObject *jsonObject = new QJsonObject();
            writeJson(currentItem,*jsonObject);
            json_root.append(*jsonObject);
        }
    }

}

QTreeWidgetItem* XmlParser::toStdItem(const QJsonArray &jarray, QString parent)
{
//    qDebug() << jarray << "|>  ARRAY  <|";
    QStringList parentToList;
    parentToList.append(parent);

    QTreeWidgetItem *result = new QTreeWidgetItem(parentToList);
    QTreeWidgetItem *item;

    foreach(const QJsonValue &value, jarray){

        if(value.isDouble()) {
            QString str = QString::fromStdString(std::to_string(value.toDouble()));
            QStringList strList;
            strList.append(str);
            item = new QTreeWidgetItem(strList);
        }

        if(value.isString()) {
            QString str = value.toString();
            QStringList strList;
            strList.append(str);
            item = new QTreeWidgetItem(strList);
        }
        if(value.isBool()) {
            QString str = value.toBool() ? "true" : "false" ;
            QStringList strList;
            strList.append(str);
            item = new QTreeWidgetItem(strList);
        }
        if(value.isObject()){
            item = toStdItem(value.toObject(), "[" + parent + "]");

        }
        if(value.isArray()){
            item = toStdItem(value.toArray(), "Array");
        }
        result->addChild(item);

    }
    return result;
}

QTreeWidgetItem *XmlParser::toStdItem(const QJsonObject &jo, QString parent)
{
//    qDebug() << jo << "|  OBJECT  |";
    QStringList parentToList;
    parentToList.append(parent);

    QTreeWidgetItem *result = new QTreeWidgetItem(parentToList);
    QTreeWidgetItem *item;
    foreach(const QString & str, jo.keys()){
        QStringList strList;
        strList.append(str);
        item = new QTreeWidgetItem(strList);
        QTreeWidgetItem *item_value;
        QString str_value;

        // Надо проврить тип данных, придется громоздко через if/else

        if(jo.value(str).isDouble()){
            str_value = QString::fromStdString(std::to_string(jo.value(str).toDouble()));
//            qDebug() << str_value << " 2 ";
            QStringList str_valueList;
            str_valueList.append(str_value);
            item_value = new QTreeWidgetItem(str_valueList);
            item->addChild(item_value);
        }
        else{
            if(jo.value(str).isString()){
                str_value = jo.value(str).toString();
//                qDebug() << str_value << " 2 ";
                QStringList str_valueList;
                str_valueList.append(str_value);
                item_value = new QTreeWidgetItem(str_valueList);
                item->addChild(item_value);
            }
            else{

                if(jo.value(str).isBool()){
                     str_value = jo.value(str).toBool() ? "true" : "false";
//                     qDebug() << str_value << " 2 ";
                     QStringList str_valueList;
                     str_valueList.append(str_value);
                     item_value = new QTreeWidgetItem(str_valueList);
                     item->addChild(item_value);
                }
                else{

                    if(jo.value(str).isArray()){
                        item = toStdItem(jo.value(str).toArray(), str);
                        item_value = new QTreeWidgetItem();
                    }

                    if(jo.value(str).isObject()){
                        item = toStdItem(jo.value(str).toObject(), str);
                        item_value = new QTreeWidgetItem();
                    }
                }
            }
        }

        result->addChild(item);

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

QDomElement XmlParser::jsonToXml(QJsonObject jsondoc, const QString name) {
    QDomElement result = document.createElement(name);
    for (const auto &key : jsondoc.keys()) {
        result.appendChild(jsonToXml(key, jsondoc.value(key)));
    }
    return result;
}

QDomElement XmlParser::jsonToXml(const QString &key, const QJsonValue &value) {
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
                node.appendChild(jsonToXml(key + QString::fromStdString( std::to_string(i)), o));
                i++;
            }
            break;

        case QJsonValue::Type::Object:
//            qDebug() << key << ": [object] ";

            node = jsonToXml(value.toObject(),key);
            break;
    }
    return node;
}
