cmake_minimum_required(VERSION 3.12)
project(pycdc)
cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Debug options.
option(ENABLE_BLOCK_DEBUG "Enable block debugging" OFF)
option(ENABLE_STACK_DEBUG "Enable stack debugging" OFF)
option(ENABLE_BINDINGS "Enable Python bindings" OFF)

# Turn debug defs on if they're enabled.
if (ENABLE_BLOCK_DEBUG)
    add_definitions(-DBLOCK_DEBUG)
endif()
if (ENABLE_STACK_DEBUG)
    add_definitions(-DSTACK_DEBUG)
endif()

# For generating the bytes tables and bindings
set(PYTHON_VENV_PATH "${CMAKE_SOURCE_DIR}/.venv")
if (EXISTS "${PYTHON_VENV_PATH}")
    message("Using existing Python venv at ${PYTHON_VENV_PATH}")
    set(Python_ROOT_DIR "${PYTHON_VENV_PATH}")
endif()

if (ENABLE_BINDINGS)
    add_subdirectory(bindings)
else()
    find_package(Python REQUIRED Interpreter)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-error=shadow -Werror ${CMAKE_CXX_FLAGS}")
elseif(MSVC)
    set(CMAKE_CXX_FLAGS "/WX ${CMAKE_CXX_FLAGS}")
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

set(PYTHON_VERSIONS
    10 11    13 14 15 16            # Python 1.1 and 1.2 are marshal-identical
    20 21 22 23 24 25 26 27
    30 31 32 33 34 35 36 37 38 39 310 311
)

foreach(ver ${PYTHON_VERSIONS})
    list(APPEND MAP_FILES ${CMAKE_CURRENT_SOURCE_DIR}/bytes/python_${ver}.map)
    list(APPEND MAP_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/bytes/python_${ver}.cpp)
endforeach()

add_custom_command(OUTPUT ${MAP_SOURCES}
                   COMMAND ${Python_EXECUTABLE}
                           ${CMAKE_CURRENT_SOURCE_DIR}/bytes/comp_map.py
                           ${CMAKE_CURRENT_SOURCE_DIR}/bytes
                           ${CMAKE_CURRENT_BINARY_DIR}/bytes
                   DEPENDS ${MAP_FILES}
                           ${CMAKE_CURRENT_SOURCE_DIR}/bytes/comp_map.py
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(pycxx STATIC
    bytecode.cpp
    data.cpp
    pyc_code.cpp
    pyc_module.cpp
    pyc_numeric.cpp
    pyc_object.cpp
    pyc_sequence.cpp
    pyc_string.cpp
    bytes/python_1_0.cpp
    bytes/python_1_1.cpp
    bytes/python_1_3.cpp
    bytes/python_1_4.cpp
    bytes/python_1_5.cpp
    bytes/python_1_6.cpp
    bytes/python_2_0.cpp
    bytes/python_2_1.cpp
    bytes/python_2_2.cpp
    bytes/python_2_3.cpp
    bytes/python_2_4.cpp
    bytes/python_2_5.cpp
    bytes/python_2_6.cpp
    bytes/python_2_7.cpp
    bytes/python_3_0.cpp
    bytes/python_3_1.cpp
    bytes/python_3_2.cpp
    bytes/python_3_3.cpp
    bytes/python_3_4.cpp
    bytes/python_3_5.cpp
    bytes/python_3_6.cpp
    bytes/python_3_7.cpp
    bytes/python_3_8.cpp
    bytes/python_3_9.cpp
    bytes/python_3_10.cpp
    bytes/python_3_11.cpp
    bytes/python_3_12.cpp
)

add_executable(pycdas pycdas.cpp)
target_link_libraries(pycdas pycxx)

install(TARGETS pycdas
    RUNTIME DESTINATION bin)

add_executable(pycdc pycdc.cpp ASTree.cpp ASTNode.cpp)
target_link_libraries(pycdc pycxx)

install(TARGETS pycdc
    RUNTIME DESTINATION bin)

add_custom_target(check "${CMAKE_CURRENT_SOURCE_DIR}/tests/all_tests.sh"
                  WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
add_dependencies(check pycdc)
