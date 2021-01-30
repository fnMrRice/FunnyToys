cmake_minimum_required(VERSION 3.14)

project(OtherLangForCpp LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Xml REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Xml REQUIRED)

set(CMAKE_CXX_FLAGS -ggdb)
set(CMAKE_CXX_FLAGS_DEBUG -ggdb)

set(SRC_PATH src)

set(STRUCT_PY 
    ${SRC_PATH}/struct_py.cpp
    ${SRC_PATH}/struct_py.h)

set(UTILS_JS
    ${SRC_PATH}/utils_js.cpp
    ${SRC_PATH}/utils_js.h)

set(ENTITY
    ${SRC_PATH}/atomic_entity.h
    ${SRC_PATH}/tuple_runtime.h
    ${SRC_PATH}/BaseEntity.h
    base_entity.cpp
    base_entity.h
    test_base_entity.cpp
    test_base_entity.h
    )

add_executable(OtherLangForCpp
    ${STRUCT_PY}
    ${UTILS_JS}
    ${ENTITY}
    main.cpp
)
target_link_libraries(OtherLangForCpp Qt${QT_VERSION_MAJOR}::Core)
