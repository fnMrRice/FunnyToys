#include <QCoreApplication>
#include <iostream>
#include "src/struct_py.h"
#include "src/TimeUtils.h"
#include "src/atomic_entity.h"
#include "test_base_entity.h"

std::string char2hex(const char &ch) {
    std::string output = "0x";
    auto constexpr hexdigits = "0123456789ABCDEF";
    output.push_back(hexdigits[ch >> 4 & 0x0F]);
    output.push_back(hexdigits[ch & 0x0F]);
    return output;
}

void printStringAsHex(const std::string &str) {
    for (uint8_t ch : str) {
        std::cout << char2hex(ch) << ' ';
    }
    std::cout << std::endl;
}

template <typename ..._Types>
void packOrPrintErr(const std::string&format, _Types...data) {
    try {
        auto result = struct_py::pack(format, data...);
        printStringAsHex(result);
    }  catch (std::exception e) {
        std::cout << e.what() << std::endl;
    }
}

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

//    int counter = 0;
//    utils_js::setTimeout([&counter] {
//        std::cout << "timeout 1s " << counter << std::endl;
//    }, 1s);
//    utils_js::setTimeout([](const int &counter) {
//        std::cout << "timeout 2s " << counter << std::endl;
//    }, 2s, counter);
//    auto id = utils_js::setInterval([&counter]() {
//        std::cout << "interval 200ms " << ++counter << std::endl;
//    }, 200ms);

//    std::this_thread::sleep_for(5s);
//    utils_js::clearInterval(id);

//    packOrPrintErr("2h", (int16_t)0x1234, (int16_t)0x4321, (int32_t)0xABCDEF12);
//    packOrPrintErr("2hll", (int16_t)0x1234, (int16_t)0x4321, (int32_t)0xABCDEF12);
//    packOrPrintErr("<2hl", (int16_t)0x1234, (int16_t)0x4321, (int32_t)0xABCDEF12);
//    packOrPrintErr(">2hl", (int16_t)0x1234, (int16_t)0x4321, (int32_t)0xABCDEF12);
//    packOrPrintErr("2b2i", (int8_t)0x1234, (int8_t)0x4321, (int32_t)0xABCDEF12, (int32_t)0x12345678);
//    packOrPrintErr("2b2i", (int8_t)0x1234, (int16_t)0x4321, (int32_t)0xABCDEF12, (int32_t)0x12345678);

    try {
        auto jsonEntity = new IBaseJsonEntity<int, std::string, std::string, int> {{"id", "name", "value", "test"}, 1, "2", "3", 4};
        auto str = jsonEntity->toString();
        std::cout << str.toStdString() << std::endl;
    }  catch (std::exception e) {
        std::cout << e.what() << std::endl;
    }

    return a.exec();
}
