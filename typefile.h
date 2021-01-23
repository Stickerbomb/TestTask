#pragma once
#ifndef TYPEFILE_H
#define TYPEFILE_H

#include <QString>



class TypeFile
{
public:
    TypeFile();
    enum class Value { Xml, Json, Undefined };
    TypeFile(const TypeFile::Value& value) : m_value(value){}
    Value value() const;
    static QString filtersList();
    static Value fromString(const QString &TypeStringName);

private:

      Value m_value = Value::Undefined;
      static const QString XmlStringName;
      static const QString JsonStringName;
};

#endif // TYPEFILE_H
