#ifndef XMLPARSER_H
#define XMLPARSER_H

#include "typefile.h"
#include <QtXml>
#include <QTreeWidgetItem>

class XmlParser
{
public:
    XmlParser();

    QTreeWidgetItem* read(const QByteArray byteArray, const TypeFile& type);
    QByteArray write(QTreeWidgetItem *source, const TypeFile& type);
    void writeXML(QTreeWidgetItem *item, QDomNode &dom_root);
private:

    void xmlToTree(const QDomNode &_elem, QTreeWidgetItem *_Model);
    void writeJson(QTreeWidgetItem *item, QJsonObject &json_root);
    void writeJson(QTreeWidgetItem *item, QJsonArray &json_root);
    QJsonValue* stringToJson(QString str);
    QTreeWidgetItem* toWidgetItem(const QJsonArray &jarray, QString parent);
    QTreeWidgetItem* toWidgetItem(const QJsonObject &jo, QString parent);
    QDomElement jsonToXml(const QString &key, const QJsonValue &value);
    QDomElement jsonToXml(QJsonObject jsondoc, const QString name);
    QJsonDocument jsondocument;
    QDomDocument document;
};

#endif // XMLPARSER_H
