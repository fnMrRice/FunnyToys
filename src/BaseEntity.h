#ifndef BASE_ENTITY_H
#define BASE_ENTITY_H
#include "atomic_entity.h"
#include <QJsonObject>
#include <QtXml/QDomElement>

template <size_t _AttributeCount, class ..._Types>
class IBaseEntity: public AtomicEntity<_AttributeCount, _Types...> {
public:
    IBaseEntity(): AtomicEntity<_AttributeCount, _Types...>({"id", "name", "value"}) {}

public:
    virtual bool fromString(char const*) = 0;
    virtual bool fromString(const std::string&) = 0;
    virtual bool fromString(const QString&) = 0;
    virtual QString toString() const = 0;
};

template <size_t _AttributeCount, class ..._Types>
class IBaseJsonEntity: public IBaseEntity<_AttributeCount, _Types...> {
public:
    IBaseJsonEntity() = default;

public:
    virtual bool fromString(char const* str) {
        auto jsonDoc = QJsonDocument::fromJson(str);
        if (!jsonDoc.isObject()) { return false; }
        return this->fromJson(jsonDoc.object());
    }
    virtual bool fromString(const std::string &str) {
        auto jsonDoc = QJsonDocument::fromJson(str.c_str());
        if (!jsonDoc.isObject()) { return false; }
        return this->fromJson(jsonDoc.object());
    }
    virtual bool fromString(const QString &str) {
        auto jsonDoc = QJsonDocument::fromJson(str.toUtf8());
        if (!jsonDoc.isObject()) { return false; }
        return this->fromJson(jsonDoc.object());
    }
    virtual QString toString() const {
        auto json = this->toJson();
        QJsonDocument temp;
        temp.setObject(json);
        return temp.toJson().data();
    }

    virtual bool fromJson(const QJsonObject &json) = 0;
    virtual QJsonObject toJson() = 0;
};

template <size_t _AttributeCount, class ..._Types>
class IBaseXmlEntity: public IBaseEntity<_AttributeCount, _Types...> {
public:
    IBaseXmlEntity() = default;

public:
    virtual bool fromString(char const*) {

    }
    virtual bool fromString(const std::string&) {}
    virtual bool fromString(const QString&) = 0;
    virtual QString toString() const {

    }
};

#endif
