cmake_minimum_required(VERSION 3.16)
project(AllTests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(TESTS_DIR ${CMAKE_SOURCE_DIR}/tests)

enable_testing()

set(CORE_HEADERS
    ${INCLUDE_DIR}/core/exceptions.hpp
    ${INCLUDE_DIR}/core/Option.hpp
    ${INCLUDE_DIR}/core/Cardinal.hpp
    ${INCLUDE_DIR}/core/DynamicArray.hpp
    ${INCLUDE_DIR}/core/Sequence.hpp
    ${INCLUDE_DIR}/core/MutableArraySequence.hpp
    ${INCLUDE_DIR}/core/Generator.hpp
)

set(GENERATOR_HEADERS
    ${INCLUDE_DIR}/generators/SequenceGenerator.hpp
    ${INCLUDE_DIR}/generators/RecurrentGenerator.hpp
    ${INCLUDE_DIR}/generators/MapGenerator.hpp
    ${INCLUDE_DIR}/generators/WhereGenerator.hpp
    ${INCLUDE_DIR}/generators/CompositeGenerator.hpp
)

set(LAZY_HEADERS
    ${INCLUDE_DIR}/lazy/LazySequence.hpp
)

# ============================================================
# GENERATOR TESTS
# ============================================================

add_executable(GeneratorTests
    ${TESTS_DIR}/GeneratorTest.cpp
    ${TESTS_DIR}/test_common.hpp
    ${CORE_HEADERS}
    ${GENERATOR_HEADERS}
)

target_include_directories(GeneratorTests PRIVATE
    ${INCLUDE_DIR}
    ${INCLUDE_DIR}/core
    ${INCLUDE_DIR}/generators
)

add_test(NAME GeneratorTests COMMAND GeneratorTests)

# ============================================================
# DYNAMICARRAY TESTS
# ============================================================

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

# ============================================================
# MUTABLEARRAYSEQUENCE TESTS
# ============================================================

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

# ============================================================
# LAZYSEQUENCE TESTS
# ============================================================

add_executable(LazySequenceTests
    ${TESTS_DIR}/LazySequenceTest.cpp
    ${TESTS_DIR}/test_common.hpp
    ${CORE_HEADERS}
    ${GENERATOR_HEADERS}
    ${LAZY_HEADERS}
)

target_include_directories(LazySequenceTests PRIVATE
    ${INCLUDE_DIR}
    ${INCLUDE_DIR}/core
    ${INCLUDE_DIR}/generators
    ${INCLUDE_DIR}/lazy
)

add_test(NAME LazySequenceTests COMMAND LazySequenceTests)

# ============================================================
# STRINGSTREAM TESTS
# ============================================================

add_executable(StringStreamTests
    ${TESTS_DIR}/StringStreamTest.cpp
    ${TESTS_DIR}/test_common.hpp
    ${INCLUDE_DIR}/streams/StringStream.hpp
    ${INCLUDE_DIR}/streams/IStream.hpp
)

target_include_directories(StringStreamTests PRIVATE
    ${INCLUDE_DIR}
    ${INCLUDE_DIR}/core
    ${INCLUDE_DIR}/streams
)

add_test(NAME StringStreamTests COMMAND StringStreamTests)

# ============================================================
# FILESTREAM TESTS
# ============================================================

add_executable(FileStreamTests
    ${TESTS_DIR}/FileStreamTest.cpp
    ${TESTS_DIR}/test_common.hpp
    ${INCLUDE_DIR}/streams/FileStream.hpp
    ${INCLUDE_DIR}/streams/IStream.hpp
)

target_include_directories(FileStreamTests PRIVATE
    ${INCLUDE_DIR}
    ${INCLUDE_DIR}/core
    ${INCLUDE_DIR}/streams
)

add_test(NAME FileStreamTests COMMAND FileStreamTests)

# ============================================================
# LAZYSEQUENCESTREAM TESTS
# ============================================================

set(LAZY_STREAM_HEADERS
    ${INCLUDE_DIR}/streams/LazySequenceStream.hpp
    ${INCLUDE_DIR}/streams/IStream.hpp
    ${INCLUDE_DIR}/lazy/LazySequence.hpp
)

add_executable(LazySequenceStreamTests
    ${TESTS_DIR}/LazySequenceStreamTest.cpp
    ${TESTS_DIR}/test_common.hpp
    ${CORE_HEADERS}
    ${GENERATOR_HEADERS}
    ${LAZY_HEADERS}
    ${LAZY_STREAM_HEADERS}
)

target_include_directories(LazySequenceStreamTests PRIVATE
    ${INCLUDE_DIR}
    ${INCLUDE_DIR}/core
    ${INCLUDE_DIR}/generators
    ${INCLUDE_DIR}/lazy
    ${INCLUDE_DIR}/streams
)

add_test(NAME LazySequenceStreamTests COMMAND LazySequenceStreamTests)

# ============================================================
# STATE TESTS
# ============================================================

add_executable(StateTests
    ${TESTS_DIR}/StateTest.cpp
    ${TESTS_DIR}/test_common.hpp
    ${INCLUDE_DIR}/statemachine/State.hpp
    ${INCLUDE_DIR}/core/Option.hpp
    ${INCLUDE_DIR}/core/exceptions.hpp
)

target_include_directories(StateTests PRIVATE
    ${INCLUDE_DIR}
    ${INCLUDE_DIR}/core
    ${INCLUDE_DIR}/statemachine
)

add_test(NAME StateTests COMMAND StateTests)