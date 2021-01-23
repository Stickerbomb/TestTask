#include "xmlparser.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QMainWindow>

XmlParser::XmlParser()
{

}

QTreeWidgetItem* XmlParser::read(const QByteArray byteArray, const TypeFile& type)
{
    QTreeWidgetItem *treeItem;
    QDomElement root;
    document.clear();
    switch(type.value()){
        case TypeFile::Value::Json:{

            qDebug() << "OPEN JSON";
            jsondocument = QJsonDocument::fromJson(byteArray);
            root = jsonToXml("Root", jsondocument.object());
            document.appendChild(root);
            break;
        }
        case TypeFile::Value::Xml:{
                qDebug() << "OPEN XML";
                document.setContent(byteArray);
                root = document.firstChild().toElement();
                break;
        }

        case TypeFile::Value::Undefined:{
                qDebug() << "OPEN I DONT KNOW";
                return nullptr;
        }
    }
    treeItem = new QTreeWidgetItem({document.firstChild().nodeName()});
    xmlToTree(root,treeItem);
    return treeItem;
}

QByteArray XmlParser::write(QTreeWidgetItem *source, const TypeFile& type)
{
    QByteArray result;
    switch(type.value()){
            case TypeFile::Value::Json:{
            QJsonDocument jsonDocument = QJsonDocument();
            QJsonObject rootObject = jsonDocument.object();
            writeJson(source,rootObject);
            qDebug() << "SAVE JSON";
            jsonDocument.setObject(rootObject); // set to json document

            result = jsonDocument.toJson();
            break;
        }
        case TypeFile::Value::Xml:{
                qDebug() << "SAVE XML";
                QDomNode root = document.createElement("Root");
                document.clear();
                writeXML(source,root);
                document.appendChild(root);

                result = document.toByteArray();
                qDebug () << root.firstChild().nodeName();
                break;
        }

        case TypeFile::Value::Undefined:{
            qDebug() << "SAVE Undefined";
            result = "";
            break;
        }
    }
    return result;
}

void XmlParser::xmlToTree(const QDomNode &_elem, QTreeWidgetItem *parentItem)
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
        QString atr = attributes.join(" ");
        QStringList name;
        name.append(domElement.nodeName());
        QTreeWidgetItem *node = new QTreeWidgetItem(name);
        if(domNode.hasAttributes()){

            QTreeWidgetItem *atr_item = new QTreeWidgetItem(attributes);
            node->addChild(atr_item);
        }
        if ((!domNode.hasChildNodes() || !domNode.firstChild().isElement()) && domElement.text() != "") {
            node->addChild(new QTreeWidgetItem({domElement.text()}));
        }
        parentItem->addChild(node);
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
            if(item->child(i)->child(1)->childCount() == 0){
                    newNodeText = document.createTextNode(item->child(i)->child(1)->text(0));
            }
            QDomElement domelem = document.createElement(item->child(i)->text(0));
            domelem.appendChild(newNodeText);
            if(item->child(i)->child(0)->childCount() == 0){
                QStringList listArguments = item->child(i)->child(0)->text(0).split(' ');
//                qDebug() << listArguments << "  ATRIBUTES  ";
                foreach(QString attr, listArguments){
                    QStringList param = attr.split('=');
                    param.first().remove('\"');
                    param.last().remove('\"');
                    domelem.setAttribute(param.first(),param.last());
                }

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
            }*/
            // end Debug section

            writeXML(item->child(i),domelem);
        }
    }


}

void XmlParser::writeJson(QTreeWidgetItem *item, QJsonObject &json_root)
{
    for(int i = 0; i< item->childCount(); i++){
        QTreeWidgetItem *currentItem = item->child(i);

        if(currentItem->childCount()>1 || currentItem->child(0)->childCount()>0){
            if(currentItem->child(0)->text(0) == currentItem->text(0) + "0"){
                QJsonArray  *jArray = new QJsonArray();
                writeJson(currentItem,*jArray);
                json_root.insert(currentItem->child(0)->text(0), *jArray);
            }
            else{
                QJsonObject *jsonObject = new QJsonObject();
                writeJson(currentItem,*jsonObject);
                json_root.insert(currentItem->child(0)->text(0 ),*jsonObject);

            }
        }
        else{
            QJsonValue *value = stringToJson(currentItem->child(0)->text(0));
            json_root.insert(currentItem->text(0),*value);

        }
    }

}

void XmlParser::writeJson(QTreeWidgetItem *item, QJsonArray &json_root)
{
    for(int i = 0; i< item->childCount(); i++){
        QTreeWidgetItem *currentItem = item->child(i);

        if(currentItem->childCount()>1 || currentItem->child(0)->childCount()>0){
            if(currentItem->child(0)->text(0) == currentItem->text(0) + "0"){
                QJsonArray  *jArray = new QJsonArray();
                writeJson(currentItem,*jArray);
                json_root.append(*jArray);
            }
            else{
                QJsonObject *jsonObject = new QJsonObject();
                writeJson(currentItem,*jsonObject);
                json_root.append(*jsonObject);

            }
        }
        else{
            QJsonValue *value = new QJsonValue(currentItem->child(0)->text(0));
            json_root.append(*value);

        }
    }

}

QTreeWidgetItem* XmlParser::toWidgetItem(const QJsonArray &jarray, QString parent)
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
            item = toWidgetItem(value.toObject(), parent);

        }
        if(value.isArray()){
            item = toWidgetItem(value.toArray(), "Array");
        }
        result->addChild(item);

    }
    return result;
}

QTreeWidgetItem *XmlParser::toWidgetItem(const QJsonObject &jo, QString parent)
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
                        item = toWidgetItem(jo.value(str).toArray(), str);
                        item_value = new QTreeWidgetItem();
                    }

                    if(jo.value(str).isObject()){
                        item = toWidgetItem(jo.value(str).toObject(), str);
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
    double dec = str.toDouble(&ok);
    if(ok){
        result= new QJsonValue(dec);
        return result;
    }
    result = new QJsonValue(str);
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
