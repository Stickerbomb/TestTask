#include "typefile.h"

const QString TypeFile::XmlStringName = "Xml files (*.xml)";
const QString TypeFile::JsonStringName = "JSON files (*.json)";

TypeFile::TypeFile()
{
}

TypeFile::Value TypeFile::value()
{
    return m_value;
}

TypeFile::Value TypeFile::fromString(const QString &TypeStringName)
{

    if (TypeStringName == TypeFile::XmlStringName) {
        return TypeFile::Value::Xml;
      } else if (TypeStringName == TypeFile::JsonStringName) {
        return TypeFile::Value::Json;
      }
      return TypeFile::Value::Undefined;
}

QString TypeFile::filtersList() {
  return TypeFile::XmlStringName + ";;" + TypeFile::JsonStringName;
}
