#include "struct_py.h"

std::tuple<struct_py::ENDIAN, bool> struct_py::ParseEndian(const char *c) {
    auto ch = *c;
    switch (ch) {
    case '@': return std::make_tuple(ENDIAN::NATIVE, true);
    case '=': return std::make_tuple(ENDIAN::NATIVE, true);
    case '<': return std::make_tuple(ENDIAN::LE, true);
    case '>': return std::make_tuple(ENDIAN::BE, true);
    case '!': return std::make_tuple(ENDIAN::NETWORK, true);
    default: return std::make_tuple(ENDIAN::NATIVE, false);
    }
}

std::queue<std::pair<struct_py::DATA_TYPE, int>> struct_py::ParseFormat(const char *c) {
    std::queue<std::pair<DATA_TYPE, int>> retval;
    size_t count = 0;
    bool hasCount = false;
    for (auto current = c; *current != '\0'; ++current) {
        auto ch = *current;
        if ('0' <= ch && ch <= '9') {
            hasCount = true;
            count *= 10;
            count += (ch - '0');
            continue;
        }
        bool isValid = true;
        switch (ch) {

#define CASE_GENERATOR(ch,type) \
case ch: \
if (!hasCount) { count=1; } \
else if (0 == count) { break; } \
retval.push(std::make_pair(DATA_TYPE::type,count)); \
break

            CASE_GENERATOR('x', SKIP);
            CASE_GENERATOR('c', CHAR);
            CASE_GENERATOR('b', S8);
            CASE_GENERATOR('B', U8);
            CASE_GENERATOR('?', BOOL);
            CASE_GENERATOR('h', S16);
            CASE_GENERATOR('H', U16);
            CASE_GENERATOR('i', S32);
            CASE_GENERATOR('I', U32);
            CASE_GENERATOR('l', S32);
            CASE_GENERATOR('L', U32);
            CASE_GENERATOR('q', S64);
            CASE_GENERATOR('Q', U64);
            CASE_GENERATOR('n', SSZ);
            CASE_GENERATOR('N', SZ);
            CASE_GENERATOR('f', F32);
            CASE_GENERATOR('d', F64);
            CASE_GENERATOR('p', PSTR);
            CASE_GENERATOR('P', POINTER);
#undef CASE_GENERATOR
        case 'e':
            throw std::runtime_error("format 'e' not supported by c++.");
        case 's':
            if (!hasCount) { count = 1; }
            retval.push(std::make_pair(DATA_TYPE::STR, count));
            break;
        default:
            isValid = false;
            break;
        }
        if (!isValid) { throw std::runtime_error("invalid character in format"); }
        hasCount = false;
        count = 0;
    }
    return retval;
}

