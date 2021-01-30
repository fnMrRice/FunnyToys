#ifndef TUPLE_RUNTIME_H
#define TUPLE_RUNTIME_H
#include <tuple>
#include <stdexcept>

// https://stackoverflow.com/questions/28997271/c11-way-to-index-tuple-at-runtime-without-using-switch
namespace detail {
template<std::size_t I>
struct visit_impl {
    template<typename Tuple, typename Function, typename ...Args>
    inline static int visit(Tuple &tuple, std::size_t idx, Function fun, Args &&...args) {
        return (idx == (I - 1U) ? (fun(std::get < I - 1U > (tuple), std::forward<Args>(args)...), void(), 0) : visit_impl < I - 1U >::visit(tuple, idx, fun, std::forward<Args>(args)...));
    }

    template<typename Return, typename Tuple, typename Function, typename ...Args>
    inline static Return visit(Tuple &tuple, std::size_t idx, Function fun, Args &&...args) {
        return (idx == (I - 1U) ? fun(std::get < I - 1U > (tuple), std::forward<Args>(args)...) : visit_impl < I - 1U >::template visit<Return>(tuple, idx, fun, std::forward<Args>(args)...));
    }
};

template<>
struct visit_impl<0U> {
    template<typename Tuple, typename Function, typename ...Args>
    [[noreturn]]
    inline static int visit(Tuple &, std::size_t, Function, Args&&...) {
        throw std::out_of_range("out of range");
    }

    template<typename Return, typename Tuple, typename Function, typename ...Args>
    [[noreturn]]
    inline static Return visit(Tuple &, std::size_t, Function, Args&&...) {
        throw std::out_of_range("out of range");
    }
};
}

template<typename Tuple, typename Function, typename ...Args>
inline void visit_at(Tuple &tuple, std::size_t idx, Function fun, Args &&...args) {
    detail::visit_impl<std::tuple_size<Tuple>::value>::visit(tuple, idx, fun, std::forward<Args>(args)...);
}

template<typename Return, typename Tuple, typename Function, typename ...Args>
inline Return visit_at(Tuple &tuple, std::size_t idx, Function fun, Args &&...args) {
    return detail::visit_impl<std::tuple_size<Tuple>::value>::template visit<Return>(tuple, idx, fun, std::forward<Args>(args)...);
}

#endif // TUPLE_RUNTIME_H
