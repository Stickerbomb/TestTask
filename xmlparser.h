#ifndef XMLPARSER_H
#define XMLPARSER_H

#include <QByteArray>

class TypeFile;
class TreeItem;
class QDomDocument;
class QDomNode;
class QDomElement;
class QDomNamedNodeMap;
class QJsonObject;
class QJsonValue;

class XmlParser
{
public:

	XmlParser();
	~XmlParser();

	TreeItem* read(const QByteArray &byteArray, const TypeFile& type);
	QByteArray write(TreeItem *rootTreeItem, const TypeFile& type);
	
private:

	void xmlToTree(const QDomNode &domNode, TreeItem *treeItem);
	void xmlToJson(const QDomNode& domNode,QJsonObject &jsonObject);
	void xmlAttributesToJson(const QDomNamedNodeMap& attributes,QJsonObject &jsonObject);
	void insertJsonValue(const QString &jsonKey, const QJsonValue &jsonValue, QJsonObject &jsonObject);

	void treeToXml(TreeItem *itemParent, QDomElement &domElementParent, QDomDocument &domDocument);
	
	QDomElement jsonToXml(const QString &jsonKey, const QJsonValue &jsonValue,QDomDocument &domDocument);
	QDomElement jsonToXml(const QString &jsonKeyParent,const QJsonObject &jsonObject,QDomDocument &domDocumnet);
};

#endif // XMLPARSER_H
