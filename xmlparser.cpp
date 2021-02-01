#include "xmlparser.h"
#include "treeitem.h"
#include "typefile.h"

#include <QDebug>
#include <QDomDocument>
#include <QDomNamedNodeMap>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

XmlParser::XmlParser()
{
}

XmlParser::~XmlParser() {
}

TreeItem *XmlParser::read(const QByteArray &byteArray, const TypeFile &type)
{
	QDomElement root;
	QDomDocument domDocument;
	switch (type.value()) {
	case TypeFile::Value::Json: {
		QJsonParseError jsonParseError;
		const auto jsonDocument = QJsonDocument::fromJson(byteArray,&jsonParseError);
		if (jsonDocument.isNull()) {
			qDebug() << jsonParseError.errorString();
			return nullptr;
		}
		root = jsonToXml("Root", jsonDocument.object(),domDocument);
		domDocument.appendChild(root);
		break;
	}
	case TypeFile::Value::Xml: {
		QString errorMsg;
		int errorLine;
		int errorColumn;	
		if (!domDocument.setContent(byteArray,&errorMsg,&errorLine,&errorColumn)) {
			qDebug() << QString("%1: error line: %2, error column: %3").arg(errorMsg).arg(errorLine).arg(errorColumn);
			return nullptr;
		}
		root = domDocument.firstChildElement();
		break;
	}
	case TypeFile::Value::Undefined: {
		qDebug() << "Unknow type of open file";
		return nullptr;
	}
	}
	
	TreeItem *rootItem = new TreeItem({domDocument.firstChildElement().nodeName()});
	xmlToTree(root, rootItem);
	return rootItem;
}

QByteArray XmlParser::write(TreeItem *rootTreeItem, const TypeFile &type)
{
	QByteArray result;
	switch (type.value()) {
	case TypeFile::Value::Json: {
		QDomDocument domDocument;
		auto rootDomNode = domDocument.createElement(rootTreeItem->data(0).toString());
		treeToXml(rootTreeItem, rootDomNode,domDocument);
		QJsonDocument jsonDocument;
		QJsonObject rootObject = jsonDocument.object();
		xmlToJson(rootDomNode,rootObject);
		jsonDocument.setObject(rootObject);
		return jsonDocument.toJson();
	}
	case TypeFile::Value::Xml: {
		QDomDocument domDocument;
		auto rootDomNode = domDocument.createElement(rootTreeItem->data(0).toString());
		treeToXml(rootTreeItem, rootDomNode,domDocument);
		domDocument.appendChild(rootDomNode);
		return domDocument.toByteArray();
	}
	case TypeFile::Value::Undefined: {
		qDebug() << "Unknow type of save file";
	}
	}
	
	return {};
}

void XmlParser::xmlToTree(const QDomNode &parentDomNode, TreeItem *parentItem) 
{
	parentItem->setData(0,parentDomNode.nodeName());
	const auto domNodeList = parentDomNode.childNodes();
 	for (int i = 0; i < domNodeList.length(); ++i) {
		const auto domNode = domNodeList.at(i);
		const auto domElement = domNode.toElement();
		TreeItem *treeItem = new TreeItem({domElement.nodeName()}, parentItem);
	
		// add attributes
		if (domNode.hasAttributes()) {
			TreeItem *treeItemAttributeValues = new TreeItem({{}}, treeItem);
			const auto attributes = domNode.attributes();
			treeItem->insertColumns(1,attributes.count());
			treeItemAttributeValues->insertColumns(1,attributes.count());
			for (int column = 0; column < attributes.count(); ++column) {
				treeItem->setData(column + 1,attributes.item(column).nodeName());
				treeItemAttributeValues->setData(column + 1,attributes.item(column).nodeValue());
			}
			treeItem->addChild(treeItemAttributeValues);
		
			if (parentItem->columnCount() < treeItem->columnCount()) {
				parentItem->insertColumns(parentItem->columnCount(),treeItem->columnCount() - parentItem->columnCount());
			}
		}
	
		// added value
		const bool hasDomChilds = (domNode.hasChildNodes() && domNode.lastChild().isElement());
		if (!hasDomChilds) {
			if (!domElement.text().isEmpty())
				treeItem->addChild(new TreeItem({domElement.text()},treeItem));
		} else {
			xmlToTree(domNode, treeItem);
			if (parentItem->columnCount() < treeItem->columnCount()) {
				parentItem->insertColumns(parentItem->columnCount(),treeItem->columnCount() - parentItem->columnCount());
			}
		}
		parentItem->addChild(treeItem);
	}
}

void XmlParser::xmlToJson(const QDomNode &parentDomNode, QJsonObject &parentJsonObject) 
{
	const auto domNodeList = parentDomNode.childNodes();
 	for (int i = 0; i < domNodeList.length(); ++i) {
		const auto domNode = domNodeList.at(i);
		const auto domElement = domNode.toElement();
		if (!domNode.hasAttributes() && domNode.childNodes().size() == 1 && domNode.lastChild().isText()) {
			insertJsonValue(domElement.nodeName(),QJsonValue(domElement.text()),parentJsonObject);
		} else if (domNode.hasAttributes() && domNode.childNodes().size() == 1 && domNode.lastChild().isText()) {
			QJsonObject jsonObject;
			jsonObject.insert(domElement.nodeName(),domElement.text());
			xmlAttributesToJson(domNode.attributes(),jsonObject);
			insertJsonValue(domElement.nodeName(),jsonObject,parentJsonObject);
		} else if (domNode.hasAttributes() && domNode.childNodes().size() == 0) {
			QJsonObject jsonObject;
			xmlAttributesToJson(domNode.attributes(),jsonObject);
			insertJsonValue(domElement.nodeName(),jsonObject,parentJsonObject);
		} else if (domNode.childNodes().size() > 1) {
			QJsonObject jsonObject;
			if (domNode.hasAttributes()) {
				xmlAttributesToJson(domNode.attributes(),jsonObject);
			}
			xmlToJson(domNode,jsonObject);
			insertJsonValue(domElement.nodeName(),jsonObject,parentJsonObject);
		}
	}
}

void XmlParser::xmlAttributesToJson(const QDomNamedNodeMap& attributes,QJsonObject &jsonObject)
{
	for (int i = 0; i < attributes.size(); ++i) {
		const auto node = attributes.item(i);
		jsonObject.insert(node.nodeName(),node.nodeValue());
	}
}

void XmlParser::insertJsonValue(const QString &jsonKey,const QJsonValue &jsonValue,QJsonObject &jsonObject)
{
	if (jsonObject.contains(jsonKey)) {
		auto jsonDuplicate = jsonObject.take(jsonKey);
		if (jsonDuplicate.isArray()) {
			auto jsonArray = jsonDuplicate.toArray();
			jsonArray.append(jsonValue);
			jsonObject.insert(jsonKey,jsonArray);
		} else {
			QJsonArray jsonArray({jsonDuplicate,{jsonValue}});
			jsonObject.insert(jsonKey,jsonArray);
		}
	} else {
		jsonObject.insert(jsonKey,jsonValue);
	}
}

void XmlParser::treeToXml(TreeItem *itemParent, QDomElement &domElementParent,QDomDocument &domDocument)
{
	for (int row = 0; row < itemParent->childCount(); ++row) {
		const auto item = itemParent->child(row);
		if (item->data(0).toString().isEmpty()) {	// skip item of attribute value
			continue;
		}
		if (item->childCount() > 0) {
			QDomElement domElement = domDocument.createElement(item->data(0).toString());
			if (item->columnCount() > 1) {	// check attributes
				for (int col = 1; col < item->columnCount(); ++col) {
					if (!item->child(0)->data(col).toString().isEmpty()) {
						domElement.setAttribute(item->data(col).toString(),item->child(0)->data(col).toString());
					}
				}
			}
			treeToXml(item,domElement,domDocument);
			domElementParent.appendChild(domElement);
		} else {
			QDomText domText = domDocument.createTextNode(item->data(0).toString());
			domElementParent.appendChild(domText);
		}
	}
}

QDomElement XmlParser::jsonToXml(const QString &jsonKeyParent, const QJsonObject &jsonObject, QDomDocument &domDocumnet)
{
	QDomElement domElement = domDocumnet.createElement(jsonKeyParent);
	for (const auto &jsonKey : jsonObject.keys()) {
		domElement.appendChild(jsonToXml(jsonKey, jsonObject.value(jsonKey),domDocumnet));
	}
	return domElement;
}

QDomElement XmlParser::jsonToXml(const QString &jsonKey, const QJsonValue &jsonValue, QDomDocument &domDocument)
{
	QDomElement domElement = domDocument.createElement(jsonKey);
	switch (jsonValue.type()) {
	case QJsonValue::Type::Bool:
		domElement.appendChild(domDocument.createTextNode(jsonValue.toBool() ? "true" : "false"));
		break;
	case QJsonValue::Type::Double:
		domElement.appendChild(domDocument.createTextNode(QString::number(jsonValue.toDouble())));
		break;
	case QJsonValue::Type::String:
		domElement.appendChild(domDocument.createTextNode(jsonValue.toString()));
		break;

	case QJsonValue::Type::Null:
	case QJsonValue::Undefined:
		domElement.appendChild({});
		break;

	case QJsonValue::Type::Array: {
		auto count = 0;
		for (const auto &arrayItem : jsonValue.toArray()) {
			domElement.appendChild(jsonToXml(QString("%1%2").arg(jsonKey).arg(count), arrayItem,domDocument));
			++count;
		}
		break;
	}

	case QJsonValue::Type::Object:
		domElement = jsonToXml(jsonKey,jsonValue.toObject(),domDocument);
		break;
	}
	
	return domElement;
}
