#ifndef XMLPARSER_H
#define XMLPARSER_H

#include <QtXml>
#include <QStandardItemModel>

class XmlParser
{
public:
    XmlParser();



    void writeJson(QStandardItem *item, QJsonObject &json_root);
    void writeJson(QStandardItem *item, QJsonArray &json_root);
    QJsonValue* stringToJson(QString str);
    QStandardItem* toStdItem(const QJsonArray &jarray, QString parent);
    QStandardItem* toStdItem(const QJsonObject &jo, QString parent);
    QDomElement jsonToDom(const QString &key, const QJsonValue &value);
    QDomElement jsonToDom(QJsonObject jsondoc, const QString name);

private:
    QJsonDocument jsondocument;
    QDomDocument document;
};

#endif // XMLPARSER_H
