#ifndef CLASSES_H
#define CLASSES_H
#include "qserializer.h"
#include <QQueue>
#include <QStack>


class Field {
    Q_GADGET
    QS_CLASS
    QS_FIELD(int, digit)
    QS_FIELD(QString, string)
    QS_FIELD(bool, flag)
    QS_FIELD(double, d_digit)
};


class Collection {
    Q_GADGET
    QS_CLASS
    QS_COLLECTION(QVector, int, vector)
    QS_COLLECTION(QList, QString, list)
    QS_COLLECTION(QStack, double, stack)
};


class CustomObject {
    Q_GADGET
    QS_CLASS
    QS_FIELD(int, digit)
    QS_COLLECTION(QVector, QString, string)
};


class CollectionOfObjects {
    Q_GADGET
    QS_CLASS
    QS_COLLECTION_OBJECTS(QVector, CustomObject, objects)
};


class General{
    Q_GADGET
    QS_CLASS
    QS_OBJECT(Field, field)
    QS_OBJECT(Collection, collection)
    QS_OBJECT(CustomObject, object)
    QS_OBJECT(CollectionOfObjects, collectionObjects)
};

class TestXmlObject {
    Q_GADGET
    QS_CLASS
    QS_FIELD(int, digit)
    QS_COLLECTION(QVector, QString, string)
};

class TestXml {
    Q_GADGET
    QS_CLASS
    QS_FIELD(int, field)
    QS_COLLECTION(QVector, int, collection)
    QS_OBJECT(TestXmlObject, object)
};



#endif // CLASSES_H
