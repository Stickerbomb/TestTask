#ifndef XMLPARSER_H
#define XMLPARSER_H

#include "typefile.h"
#include <QtXml>
#include <QStandardItemModel>
#include <QTreeWidgetItem>

class XmlParser
{
public:
    XmlParser();


    QTreeWidgetItem* read(const QByteArray byteArray,  TypeFile& type);
    void writeToFile(QString filename, TypeFile& type);
    void writeXML(QTreeWidgetItem *item, QDomNode &dom_root);
private:

    void xmlToTree(const QDomNode &_elem, QTreeWidgetItem *_Model);
    void writeJson(QTreeWidgetItem *item, QJsonObject &json_root);
    void writeJson(QTreeWidgetItem *item, QJsonArray &json_root);
    QJsonValue* stringToJson(QString str);
    QTreeWidgetItem* toStdItem(const QJsonArray &jarray, QString parent);
    QTreeWidgetItem* toStdItem(const QJsonObject &jo, QString parent);
    QDomElement jsonToXml(const QString &key, const QJsonValue &value);
    QDomElement jsonToXml(QJsonObject jsondoc, const QString name);
    QJsonDocument jsondocument;
    QDomDocument document;
};

#endif // XMLPARSER_H
