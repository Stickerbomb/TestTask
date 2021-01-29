#include "xmlparser.h"
#include "typefile.h"
#include "treeitem.h"

XmlParser::XmlParser()
{

}

XmlParser::~XmlParser()
{
    jsondocument.~QJsonDocument();
    document.~QDomDocument();
}

TreeItem* XmlParser::read(const QByteArray byteArray, const TypeFile& type)
{
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
            //bool QDomDocument::setContent(const QByteArray &data, bool namespaceProcessing, QString *errorMsg = nullptr, int *errorLine = nullptr, int *errorColumn = nullptr)

            document.setContent(byteArray);
            root = document.firstChildElement();
            break;
        }

        case TypeFile::Value::Undefined:{
            qDebug() << "OPEN I DONT KNOW";
            return nullptr;
        }
    }
    TreeItem *rootItem = new TreeItem(document.firstChildElement().nodeName());
    xmlToTree(root,rootItem);
    return rootItem;
}

QByteArray XmlParser::write(TreeItem *source, const TypeFile& type)
{
    QByteArray result;
    switch(type.value()){
            case TypeFile::Value::Json:{
            QJsonDocument jsonDocument = QJsonDocument();
            QJsonObject rootObject = jsonDocument.object();
            qDebug() << "SAVE JSON";
            writeJson(source,rootObject);
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

void XmlParser::xmlToTree(const QDomNode &_elem, TreeItem *parentItem)
{
    parentItem->setData(_elem.nodeName());
    const auto domNodeList = _elem.childNodes();
    for (int i = 0; i < domNodeList.length(); ++i) {
        const auto domNode = domNodeList.at(i);
        QDomElement domElement = domNode.toElement();
        const QDomNamedNodeMap attributeMap = domNode.attributes();
        TreeItem *treeItem = new TreeItem({QVariant(domElement.nodeName())},parentItem);;
        if(domNode.hasAttributes()){
            QStringList attributes;
            for (int i = 0; i < attributeMap.count(); ++i) {
                QDomNode attribute = attributeMap.item(i);
                attributes << attribute.nodeName() + "=\"" + attribute.nodeValue() + '"';
            }
            QString atr = attributes.join(" ");
            treeItem = new TreeItem({QVariant(domElement.nodeName())},parentItem);
            TreeItem *item = new TreeItem({QVariant(atr)},treeItem);
            treeItem->addChild(item);
        }

        if ((!domNode.hasChildNodes() || !domNode.lastChild().isElement()) && domElement.text() != "") {

            TreeItem *item = new TreeItem({QVariant(domElement.text())},treeItem);
            treeItem->addChild(item);
        }
        parentItem->addChild(treeItem);
        if (domNode.hasChildNodes() && domNode.lastChild().isElement()) {
            xmlToTree(domNode, treeItem);
        }
    }
}

void XmlParser::writeXML(TreeItem *item, QDomNode &dom_root)
{

    for(int i = 0; i< item->childCount(); i++){
        TreeItem *currentItem = item->child(i);

        if(currentItem->childCount()>0){
            QDomElement domelem = document.createElement(currentItem->data().toString());
            if(currentItem->child(0)->childCount()==0){
                QStringList listArguments = currentItem->child(0)->data().toString().split(' ');
                foreach(QString attr, listArguments){
                    QStringList param = attr.split('=');
                    param.first().remove('\"');
                    param.last().remove('\"');
                    domelem.setAttribute(param.first(),param.last());
                }
            }
            dom_root.appendChild(domelem);
            writeXML(item->child(i),domelem);
        }
        else{
            if(i!=0){
                QDomText newNodeText = document.createTextNode(currentItem->data().toString());
                dom_root.appendChild(newNodeText);
            }
        }
    }


}

void XmlParser::writeJson(TreeItem *item, QJsonObject &json_root)
{

    for(int i = 0; i< item->childCount(); i++){
        TreeItem *currentItem = item->child(i);
        if(currentItem->childCount()>0){

                if(currentItem->child(0)->data().toString() == currentItem->data().toString() + "0"){
                    QJsonArray  *jArray = new QJsonArray();
                    writeJson(currentItem,*jArray);
                    if(currentItem->child(0)->childCount()==0){
                        QStringList listArguments = currentItem->child(0)->data().toString().split(' ');
                        foreach(QString attr, listArguments){
                            QStringList param = attr.split('=');
                            QJsonValue *value = stringToJson(param.last().remove('\"'));
                            jArray->append(*value);
                        }
                    }
                    json_root.insert(currentItem->child(0)->data().toString().remove('\"'), *jArray);
                }
                else{
                    QJsonObject *jsonObject = new QJsonObject();
                    writeJson(currentItem,*jsonObject);
                    if(currentItem->child(0)->childCount()==0){
                        QStringList listArguments = currentItem->child(0)->data().toString().split(' ');
                        foreach(QString attr, listArguments){
                            QStringList param = attr.split('=');
                            QJsonValue *value = stringToJson(param.last().remove('\"'));
                            jsonObject->insert(param.first().remove('\"'),*value);
                        }
                    }
                    json_root.insert(currentItem->data().toString().remove('\"'),*jsonObject);
                }
        }
    }

}

void XmlParser::writeJson(TreeItem *item, QJsonArray &json_root)
{
    for(int i = 0; i < item->childCount(); i++){
        TreeItem *currentItem = item->child(i);

        if(currentItem->childCount()>0){// (childCount() > 0) вместо (hasChildren())
            if(currentItem->child(0)->childCount()==0){
                    QStringList listArguments = currentItem->child(0)->data().toString().split(' ');
                    foreach(QString attr, listArguments){
                        QStringList param = currentItem->child(0)->data().toString().split('=');
                        QJsonValue *value = stringToJson(param.last().remove('\"'));
                        json_root.append(*value);
                        }
            }
            else{
                if(currentItem->child(0)->data().toString() == currentItem->data().toString() + "0"){
                    QJsonArray  *jArray = new QJsonArray();
                    writeJson(currentItem,*jArray);
                    json_root.append( *jArray);
                }
                else{
                    QJsonObject *jsonObject = new QJsonObject();
                    writeJson(currentItem,*jsonObject);
                    json_root.append(*jsonObject);

                }
            }
        }

    }
}

TreeItem* XmlParser::jsonToTreeItem(const QJsonArray &jarray, QString parent)
{;

    TreeItem *result = new TreeItem({QVariant(parent)});
    TreeItem *item;

    foreach(const QJsonValue &value, jarray){

        if(value.isDouble()) {
            QString str = QString::fromStdString(std::to_string(value.toDouble()));
            item = new TreeItem({QVariant(str)});
        }

        if(value.isString()) {
            QString str = value.toString();
            item = new TreeItem({QVariant(str)});
        }
        if(value.isBool()) {
            QString str = value.toBool() ? "true" : "false" ;
            item = new TreeItem({QVariant(str)});
        }
        if(value.isObject()){
            item = jsonToTreeItem(value.toObject(), parent);

        }
        if(value.isArray()){
            item = jsonToTreeItem(value.toArray(), "Array");
        }
        result->addChild(item);

    }
    return result;
}

TreeItem *XmlParser::jsonToTreeItem(const QJsonObject &jo, QString parent)
{

    TreeItem *result = new TreeItem(parent);
    TreeItem *item;
    foreach(const QString & str, jo.keys()){
        item = new TreeItem({QVariant(str)});
        TreeItem *item_value;
        QString str_value;


        if(jo.value(str).isDouble()){
            str_value = QString::fromStdString(std::to_string(jo.value(str).toDouble()));
            item_value = new TreeItem({QVariant(str_value)});
            item->addChild(item_value);
        }
        else{
            if(jo.value(str).isString()){
                str_value = jo.value(str).toString();
                item_value = new TreeItem({QVariant(str_value)});
                item->addChild(item_value);
            }
            else{

                if(jo.value(str).isBool()){
                     str_value = jo.value(str).toBool() ? "true" : "false";
                     item_value = new TreeItem({QVariant(str_value)});
                     item->addChild(item_value);
                }
                else{

                    if(jo.value(str).isArray()){
                        item = jsonToTreeItem(jo.value(str).toArray(), str);
                        item_value = new TreeItem({QVariant("")});
                    }

                    if(jo.value(str).isObject()){
                        item = jsonToTreeItem(jo.value(str).toObject(), str);
                        item_value = new TreeItem({QVariant("")});
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
    QJsonValue *result;
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
    return result;
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
            node.setAttribute(key,QString::fromStdString( std::to_string( value.toBool())));
            break;

        case QJsonValue::Type::Double:
            node.setAttribute(key, QString::fromStdString( std::to_string(value.toDouble())));
            break;

        case QJsonValue::Type::String:
            node.setAttribute(key, value.toString());
            break;

        case QJsonValue::Type::Null:
            node.appendChild( document.createElement( "[null]"));
            break;

        case QJsonValue::Type::Array:
            for (const auto &o : value.toArray()) {
                node.appendChild(jsonToXml(key + QString::fromStdString( std::to_string(i)), o));
                i++;
            }
            break;

        case QJsonValue::Type::Object:{

            node = jsonToXml(value.toObject(),key);
            break;
    }
    }
    return node;
}
