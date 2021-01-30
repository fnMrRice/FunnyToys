#ifndef STRUCT_PY_H
#define STRUCT_PY_H
#include <tuple>
#include <vector>
#include <string>
#include <queue>
#include <stdexcept>
#include <cstdint>

class struct_py {
public:
    enum class ENDIAN {
        BE, LE, NATIVE,
        NETWORK = BE,
        X64_86 = LE, ARM = BE,
    };

    enum class DATA_TYPE {
        PASS, SKIP = PASS,
        CHAR, S8, U8, BOOL, S16, U16, S32, U32, S64, U64,
        SSZ, SZ, F16, F32, F64, STR, PSTR, POINTER,
        NOT_SUPPORTED
    };

    using byte = unsigned char;

    class format_size_not_match: public std::runtime_error {
    public:
        format_size_not_match(const char *what): std::runtime_error(what) {}
    };

    class format_type_not_match: public std::runtime_error {
    public:
        format_type_not_match(const char *what): std::runtime_error(what) {}
    };

public:
    template<typename ..._Rest>
    static std::string pack(const std::string &format, const _Rest& ...args);

    static auto unpack(const char *format) {

    }

private:
    static std::tuple<ENDIAN, bool> ParseEndian(const char *c);
    static std::queue<std::pair<DATA_TYPE, int>> ParseFormat(const char *c);

    template<typename _T, ENDIAN endian, size_t len = sizeof(_T)>
    static std::array<byte, len> pack_single(const _T &data);

    template<ENDIAN endian>
    static std::string pack_by_queue(std::queue<std::pair<DATA_TYPE, int>> &format);

    template<ENDIAN endian, typename _T, typename ..._Rest>
    static std::string pack_by_queue(std::queue<std::pair<DATA_TYPE, int>> &format, const _T &arg1, const _Rest& ...args);

private:
    explicit struct_py() = delete;
    explicit struct_py(const struct_py &) = delete;
    virtual ~struct_py() = delete;
};

template<typename ..._Rest>
std::string struct_py::pack(const std::string &format, const _Rest& ...args) {
    auto [endian, notDefault] = ParseEndian(format.c_str());
    auto q_format = ParseFormat(format.c_str() + notDefault);
    if (endian == ENDIAN::LE) { return pack_by_queue<ENDIAN::LE>(q_format, args...); }
    if (endian == ENDIAN::BE) { return pack_by_queue<ENDIAN::BE>(q_format, args...); }
    if (endian == ENDIAN::NATIVE) { return pack_by_queue<ENDIAN::NATIVE>(q_format, args...); }
    throw std::runtime_error("unknown error.");
}

template<typename _T, struct_py::ENDIAN endian, size_t len>
std::array<struct_py::byte, len> struct_py::pack_single(const _T &data) {
    std::array<byte, len> retval = {0};
    for (size_t i = 0; i < len; i++) {
        if constexpr (endian == ENDIAN::BE) {
            retval[len - i - 1] = ((data >> (8 * i)) & 0xFF);
        }
        if constexpr (endian == ENDIAN::LE) {
            retval[i] = ((data >> (8 * i)) & 0xFF);
        }
        if constexpr (endian == ENDIAN::NATIVE) {
            retval[i] = reinterpret_cast<const char*>(&data)[i];
        }
    }
    return retval;
}

template<struct_py::ENDIAN endian>
std::string struct_py::pack_by_queue(std::queue<std::pair<struct_py::DATA_TYPE, int>> &format) {
    if (!format.empty()) { throw format_size_not_match("format too long"); }
    return "";
}

template<struct_py::ENDIAN endian, typename _T, typename ..._Rest>
std::string struct_py::pack_by_queue(std::queue<std::pair<struct_py::DATA_TYPE, int>> &format, const _T &arg1, const _Rest& ...args) {
    if (format.empty()) {
        throw format_size_not_match("format too short");
    }
    auto &fmt = format.front();

    std::string retval;
    bool canParse = true;

    auto push = [&] {
        for (auto ch : pack_single<_T, endian>(arg1)) {
            retval.push_back(ch);
        }
    };

    switch (fmt.first) {
#define CODEGEN(A,B,C) \
    case DATA_TYPE::A: \
        if (!std::is_same_v<B, _T>) { throw format_type_not_match("param type not match " #C); } \
        push(); \
        break

        CODEGEN(CHAR, char, char);
        CODEGEN(BOOL, bool, bool);
        CODEGEN(STR, char, char);
        CODEGEN(S8, int8_t, int8_t);
        CODEGEN(U8, uint8_t, uint8_t);
        CODEGEN(S16, int16_t, int16_t);
        CODEGEN(U16, uint16_t, uint16_t);
        CODEGEN(S32, int32_t, int32_t);
        CODEGEN(U32, uint32_t, uint32_t);
        CODEGEN(S64, int64_t, int64_t);
        CODEGEN(U64, uint64_t, uint64_t);
        CODEGEN(F32, float, float32_t);
        CODEGEN(F64, double, float64_t);
        CODEGEN(SSZ, int64_t, ssize_t);
        CODEGEN(SZ, size_t, size_t);
        CODEGEN(POINTER, void*, void*);
#undef CODEGEN
    case DATA_TYPE::PASS: break;
    case DATA_TYPE::F16:
        throw format_type_not_match("param type not match float16_t");
        push();
        break;
    case DATA_TYPE::PSTR: {
        if (fmt.second < 0 || 255 < fmt.second) { throw std::out_of_range("Pascal string too long"); }
        retval.push_back((unsigned char)fmt.second);
        fmt.first = DATA_TYPE::STR;
        break;
    }
    default:
        throw format_type_not_match("invalid format");
    }
    --fmt.second;
    if (fmt.second == 0) {
        format.pop();
    }
    return retval + pack_by_queue<endian>(format, args...);
}

#endif // STRUCT_PY_H
