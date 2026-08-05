cmake_minimum_required(VERSION 3.16)
project(SequenceTests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(TESTS_DIR ${CMAKE_SOURCE_DIR}/tests)

set(CORE_HEADERS
    ${INCLUDE_DIR}/core/exceptions.hpp
    ${INCLUDE_DIR}/core/DynamicArray.hpp
    ${INCLUDE_DIR}/core/Sequence.hpp
    ${INCLUDE_DIR}/core/MutableArraySequence.hpp
)

add_executable(MutableArraySequenceTests
    ${TESTS_DIR}/MutableArraySequenceTest.cpp
    ${TESTS_DIR}/test_common.hpp
    ${CORE_HEADERS}
)

target_include_directories(MutableArraySequenceTests PRIVATE
    ${INCLUDE_DIR}
    ${INCLUDE_DIR}/core
)

add_test(NAME MutableArraySequenceTests COMMAND MutableArraySequenceTests)

add_executable(DynamicArrayTests
    ${TESTS_DIR}/DynamicArrayTest.cpp
    ${TESTS_DIR}/test_common.hpp
    ${INCLUDE_DIR}/core/exceptions.hpp
    ${INCLUDE_DIR}/core/DynamicArray.hpp
)

target_include_directories(DynamicArrayTests PRIVATE
    ${INCLUDE_DIR}
    ${INCLUDE_DIR}/core
)

add_test(NAME DynamicArrayTests COMMAND DynamicArrayTests)