#ifndef XMLPARSER_H
#define XMLPARSER_H

#include <QtXml>

class TypeFile;
class TreeItem;

class XmlParser
{
public:
    XmlParser();
    ~XmlParser();

    TreeItem* read(const QByteArray byteArray, const TypeFile& type);
    QByteArray write(TreeItem *source, const TypeFile& type);
    void writeXML(TreeItem *item, QDomNode &dom_root);
private:

    void xmlToTree(const QDomNode &_elem, TreeItem *_item);
    void writeJson(TreeItem *item, QJsonObject &json_root);
    void writeJson(TreeItem *item, QJsonArray &json_root);
    QJsonValue* stringToJson(QString str);
    TreeItem* toWidgetItem(const QJsonArray &jarray, QString parent);
    TreeItem* toWidgetItem(const QJsonObject &jo, QString parent);
    QDomElement jsonToXml(const QString &key, const QJsonValue &value);
    QDomElement jsonToXml(QJsonObject jsondoc, const QString name);
    QJsonDocument jsondocument;
    QDomDocument document;
};

#endif // XMLPARSER_H
