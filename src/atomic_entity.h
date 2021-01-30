#ifndef ATOMIC_ENTITY_H
#define ATOMIC_ENTITY_H
#include "tuple_runtime.h"
#include <string>
#include <array>
#include <tuple>
#include <stdexcept>

class IAtomicEntity {
protected:
    explicit IAtomicEntity() = default;
    explicit IAtomicEntity(const IAtomicEntity&) = default;
    virtual ~IAtomicEntity() = default;
};

template <class _T>
class attribute_type_error: public std::runtime_error {
public:
    attribute_type_error(): std::runtime_error("attribute_type_error") {}
    attribute_type_error(char const* what): std::runtime_error(what) {}
    attribute_type_error(const std::string &what): std::runtime_error(what) {}
};

template <size_t _AttributeCount, class ..._Types>
class AtomicEntity: public IAtomicEntity {
public:
    AtomicEntity(const std::array<std::string, _AttributeCount> &AttributeNames);
    AtomicEntity(const std::array<std::string, _AttributeCount> &AttributeNames, _Types... init_values);
    virtual ~AtomicEntity() {}

public:
    template<class _T>
    void set(const size_t index, const _T &value) {
        static auto visitor = [&value]<class _RType>(_RType & v) {
            if constexpr (std::is_same_v<_RType, _T>) {
                v = value;
            } else {
                throw attribute_type_error<_RType>();
            }
        };
        visit_at(m_values, index, visitor);
        m_has_value[index] = true;
    }

    template<class _T>
    void set(const std::string &name, const _T &value) {
        auto [success, index] = findIndexByName(name);
        if (!success) { throw std::out_of_range(std::string("cannot find attribute: ") + name); }
        this->set<_T>(index, value);
    }

    template<class _T>
    auto at(const size_t &index) const {
        static constexpr auto const visitor_with_result = [this, &index]<class _RType>(_RType v) -> _T {
            if (!m_has_value[index]) { throw attribute_type_error<_RType>("value has not been set yet."); }
            if constexpr (std::is_same_v<_RType, _T>) {
                return v;
            } else {
                throw attribute_type_error<_RType>();
            }
        };
        return visit_at<_T>(m_values, index, visitor_with_result);
    }

    template<class _T>
    auto at(const std::string &name) const {
        auto [success, index] = findIndexByName(name);
        if (!success) { throw std::out_of_range(std::string("cannot find attribute: ") + name); }
        return this->at<_T>(index);
    }

public:
    std::array<std::string, _AttributeCount> const &name(const size_t &index) const { return m_names.at(index); }
    std::array<std::string, _AttributeCount> const &names() const { return m_names; }
    std::tuple<_Types...> const &values() const { return m_values; }

protected:
    template<size_t _Index, class _Type>
    static constexpr bool isSame() {
        return std::is_same_v<std::tuple_element_t<_Index, std::tuple<_Types...>>, _Type>;
    }

private:
    template<class _Type, class ..._Rest>
    static inline constexpr void checkTypes() {
        if constexpr (std::is_pointer_v<_Type>) {
            static_assert (std::is_convertible_v<_Type, IAtomicEntity*>, "cannot use pointer of non-entity");
        } else {
            static_assert(std::is_arithmetic_v<_Type> || std::is_same_v<_Type, std::string>, "cannot only use string or arithmetic type");
            if constexpr (0 < sizeof...(_Rest)) { checkTypes<_Rest...>(); }
        }
    }

    std::tuple<bool, size_t> findIndexByName(const std::string &name) const {
        size_t index = 0;
        for (auto const &temp : m_names) {
            if (name == temp) {
                return std::make_tuple(true, index);
                break;
            }
            ++index;
        }
        return std::make_tuple(false, 0);
    }

private:
    std::array<std::string, _AttributeCount> m_names;
    std::array<bool, _AttributeCount> m_has_value;
    std::tuple<_Types...> m_values;
};

template <size_t _AttributeCount, class ..._Types> AtomicEntity<_AttributeCount, _Types...>::AtomicEntity(const std::array<std::string, _AttributeCount> &AttributeNames) {
    static_assert(_AttributeCount <= sizeof...(_Types), "Attribute count not matches Types given");
    static_assert(_AttributeCount != 0, "Attribute count cannot be 0");
    checkTypes<_Types...>();
    m_names = AttributeNames;
    for (auto &value : m_has_value) {
        value = false;
    }
}

template <size_t _AttributeCount, class ..._Types> AtomicEntity<_AttributeCount, _Types...>::AtomicEntity(const std::array<std::string, _AttributeCount> &AttributeNames, _Types... init_values) {
    static_assert(_AttributeCount <= sizeof...(_Types), "Attribute count not matches Types given");
    static_assert(_AttributeCount != 0, "Attribute count cannot be 0");
    checkTypes<_Types...>();
    m_names = AttributeNames;
    m_values = std::make_tuple(init_values...);
    for (auto &value : m_has_value) {
        value = true;
    }
}

#endif // ATOMIC_ENTITY_H
