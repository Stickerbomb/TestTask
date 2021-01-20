#ifndef XMLPARSER_H
#define XMLPARSER_H

#include "typefile.h"
#include <QtXml>
#include <QStandardItemModel>

class XmlParser
{
public:
    XmlParser();


    QStandardItem* read(const QByteArray byteArray,  TypeFile& type);
    void writeToFile(QString filename, TypeFile& type);
    void writeXML(QStandardItem *item, QDomNode &dom_root);
private:

    void xmlToTree(const QDomNode &_elem, QStandardItem *_Model);
    void writeJson(QStandardItem *item, QJsonObject &json_root);
    void writeJson(QStandardItem *item, QJsonArray &json_root);
    QJsonValue* stringToJson(QString str);
    QStandardItem* toStdItem(const QJsonArray &jarray, QString parent);
    QStandardItem* toStdItem(const QJsonObject &jo, QString parent);
    QDomElement jsonToXml(const QString &key, const QJsonValue &value);
    QDomElement jsonToXml(QJsonObject jsondoc, const QString name);
    QJsonDocument jsondocument;
    QDomDocument document;
};

#endif // XMLPARSER_H
