#include "xmlparser.h"

XmlParser::XmlParser()
{

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


        qDebug() << item->text() << item_value->text() << " 11 ";
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
    qDebug() << result.nodeName() << "added OBJECT";
    return result;
}

QDomElement XmlParser::jsonToDom(const QString &key, const QJsonValue &value) {
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
