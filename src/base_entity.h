#ifndef BASE_ENTITY_H
#define BASE_ENTITY_H
#include "src/atomic_entity.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <iostream>

template <class ..._Types>
class IBaseEntity {
    struct _AttributeCount {
        static size_t constexpr value = sizeof...(_Types);
    };
protected:
    static inline constexpr size_t size_v = _AttributeCount::value;
public:
    IBaseEntity(const std::array<std::string, size_v> &AttributeNames): m_atomic_entity(AttributeNames) {};
    IBaseEntity(const std::array<std::string, size_v> &AttributeNames, _Types... values): m_atomic_entity(AttributeNames, values...) {};

public:
    virtual bool fromString(char const*) = 0;
    virtual bool fromString(const std::string&) = 0;
    virtual bool fromString(const QString&) = 0;
    virtual QString toString() const = 0;

protected:
    template<class _T>
    void set(const size_t index, const _T &value) {
        std::cout << "set " << index << " values " << value << std::endl;
        return m_atomic_entity.set(index, value);
    }

    template<class _T>
    void set(const std::string name, const _T &value) {
        return m_atomic_entity.set(name, value);
    }

    template<class _T>
    auto at(const size_t &index) const {
        return m_atomic_entity.template at<_T>(index);
    }

    template<class _T>
    auto at(const std::string &name) const {
        return m_atomic_entity.template at<_T>(name);
    }

    auto const &name(const size_t &index) const { return m_atomic_entity.name(index); }
    auto const &names() const { return m_atomic_entity.names(); }
    auto const &values() const { return m_atomic_entity.values(); }

private:
    AtomicEntity<size_v, _Types...> m_atomic_entity;
};

template <class ..._Types>
class IBaseJsonEntity: public IBaseEntity<_Types...> {
private:
    static inline size_t constexpr size_v = IBaseEntity<_Types...>::size_v;
public:
    IBaseJsonEntity(const std::array<std::string, size_v> &AttributeNames): IBaseEntity<_Types...>(AttributeNames) {};
    IBaseJsonEntity(const std::array<std::string, size_v> &AttributeNames, _Types... values): IBaseEntity<_Types...>(AttributeNames, values...) {};

public:
#define ATTRIBUTE(type,name) \
    public:  void set_##name(const type &value) { this->set(#name,value); } \
    private: type get_##name() const { return this->template at<type>(#name); }
#undef ATTRIBUTE

public:
    virtual bool fromString(char const* str) override;
    virtual bool fromString(const std::string &str) override;
    virtual bool fromString(const QString &str) override;
    virtual QString toString() const override;

    virtual bool fromJson(const QJsonObject &json);
    virtual QJsonObject toJson() const;

protected:
    static bool GetBool(const QJsonObject &obj, const QString &key, bool &out, const bool &optional = false, const bool &nullable = false) {
        auto const &value = obj.value(key);
        if (optional && value.isUndefined()) { return true; }
        if (nullable && value.isNull()) { return true; }
        if (value.isBool()) { out = value.toBool(); }
        else if (value.isString()) {
            auto temp = value.toString();
            if (temp == "true") { out = true; }
            else if (temp == "false") { out = false; }
            return false;
        } else if (value.isDouble()) {
            auto temp = value.toInt();
            if (temp == 0) { out = false; }
            if (temp == 1) { out = true; }
        } else { return false; }
        return true;
    }

    static bool GetInt(const QJsonObject &obj, const QString &key, int &out, const bool &optional = false, const bool &nullable = false) {
        auto const &value = obj.value(key);
        if (optional && value.isUndefined()) { return true; }
        if (nullable && value.isNull()) { return true; }
        if (value.isString()) {
            bool success;
            auto temp = value.toString().toInt(&success);
            if (!success) { return false; }
            out = temp;
        } else if (value.isDouble()) { out = value.toInt(); }
        else { return false; }
        return true;
    }

    static bool GetDouble(const QJsonObject &obj, const QString &key, double &out, const bool &optional = false, const bool &nullable = false) {
        auto const &value = obj.value(key);
        if (optional && value.isUndefined()) { return true; }
        if (nullable && value.isNull()) { return true; }
        if (value.isString()) {
            bool success;
            auto temp = value.toString().toDouble(&success);
            if (!success) { return false; }
            out = temp;
        } else if (value.isDouble()) { out = value.toDouble(); }
        else { return false; }
        return true;
    }

    static bool GetArray(const QJsonObject &obj, const QString &key, QJsonArray &out, const bool &optional = false, const bool &nullable = false) {
        auto const &value = obj.value(key);
        if (optional && value.isUndefined()) { return true; }
        if (nullable && value.isNull()) { return true; }
        if (value.isArray()) { out = value.toArray(); }
        else { return false; }
        return true;
    }

    static bool GetObject(const QJsonObject &obj, const QString &key, QJsonObject &out, const bool &optional = false, const bool &nullable = false) {
        auto const &value = obj.value(key);
        if (optional && value.isUndefined()) { return true; }
        if (nullable && value.isNull()) { return true; }
        if (value.isObject()) { out = value.toObject(); }
        else { return false; }
        return true;
    }

private:
    template<size_t _Index>
    bool getKeyFromJsonObject(const QJsonObject &object) {
        auto name = std::get<_Index>(this->names());
        if (!object.contains(QString::fromStdString(name))) { return false; }
        if constexpr (std::is_same_v<std::tuple_element_t<_Index, std::tuple<_Types...>>, std::string>) {
            this->set(_Index, object.value(QString::fromStdString(name)).toString().toStdString());
        } else {
            this->set(_Index, object.value(QString::fromStdString(name)).toInt());
        }
        return true;
    }

    template<size_t _Index>
    void insertKeyToJsonObject(QJsonObject &object) const {
        auto name = std::get<_Index>(this->names());
        auto value = std::get<_Index>(this->values());
        if constexpr (std::is_same_v<std::tuple_element_t<_Index, std::tuple<_Types...>>, std::string>) {
            object.insert(QString::fromStdString(name), QString::fromStdString(value));
        } else {
            object.insert(QString::fromStdString(name), value);
        }
    }

    template<size_t _Index>
    bool getKeyRecursivly(const QJsonObject &object) {
        if (!getKeyFromJsonObject<_Index>(object)) { return false; }
        if constexpr (_Index == 0) { return true; }
        else { return getKeyRecursivly < _Index - 1 > (object); }
    }

    template<size_t _Index>
    void insertKeyRecursivly(QJsonObject &object) const {
        insertKeyToJsonObject<_Index>(object);
        if constexpr (_Index == 0) { return; }
        else { insertKeyRecursivly < _Index - 1 > (object); }
    }
};

#define TEMPLATE_COMMON(type,klass) \
template <class ..._Types> type klass<_Types...>

TEMPLATE_COMMON(bool, IBaseJsonEntity)::fromString(char const* str) {
    auto jsonDoc = QJsonDocument::fromJson(str);
    if (!jsonDoc.isObject()) { return false; }
    return this->fromJson(jsonDoc.object());
}

TEMPLATE_COMMON(bool, IBaseJsonEntity)::fromString(const std::string &str) {
    auto jsonDoc = QJsonDocument::fromJson(str.c_str());
    if (!jsonDoc.isObject()) { return false; }
    return this->fromJson(jsonDoc.object());
}

TEMPLATE_COMMON(bool, IBaseJsonEntity)::fromString(const QString &str) {
    auto jsonDoc = QJsonDocument::fromJson(str.toUtf8());
    if (!jsonDoc.isObject()) { return false; }
    return this->fromJson(jsonDoc.object());
}

TEMPLATE_COMMON(QString, IBaseJsonEntity)::toString() const {
    auto json = this->toJson();
    QJsonDocument temp;
    temp.setObject(json);
    return temp.toJson();
}

TEMPLATE_COMMON(bool, IBaseJsonEntity)::fromJson(const QJsonObject &json) {
    return getKeyRecursivly < size_v - 1 > (json);
}

TEMPLATE_COMMON(QJsonObject, IBaseJsonEntity)::toJson() const {
    QJsonObject retval;
    insertKeyRecursivly < size_v - 1 > (retval);
    return retval;
}

#undef TEMPLATE_COMMON

#endif
