cmake_minimum_required(VERSION 3.16)
project(StateMachineLab)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/source)
set(TESTS_DIR ${CMAKE_SOURCE_DIR}/tests)


set(ALL_HEADERS "")
set(ALL_SOURCES "")

add_executable(${PROJECT_NAME}
    ${ALL_SOURCES}
    ${ALL_HEADERS}
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${INCLUDE_DIR}
    ${INCLUDE_DIR}/core
    ${INCLUDE_DIR}/streams
    ${INCLUDE_DIR}/lazy
    ${INCLUDE_DIR}/statemachine
    ${INCLUDE_DIR}/widgets
    ${INCLUDE_DIR}/utils
)

target_link_libraries(${PROJECT_NAME}
    Qt6::Widgets
    Threads::Threads
)