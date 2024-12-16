cmake_minimum_required(VERSION 3.16)

project(HttpParser
  VERSION 0.1
  DESCRIPTION "Tcl Pico Http Parser extension"
  LANGUAGES C)

include(CTest)


add_library(HttpParser SHARED
    src/picohttpparser.h
    src/picohttpparser.c
    src/lib.c
    src/pathparser.h
    src/pathparser.c
    )

if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
    add_subdirectory(HttpParserTests)
endif ()

enable_testing()

set_target_properties(HttpParser PROPERTIES
    OUTPUT_NAME "http_parser")

add_compile_options(-Wall -fsanitize=address,undefined)
target_compile_options(HttpParser PRIVATE -Wno-incompatible-pointer-types)

 # CMake knows how to find TCL
find_package(TCL)
find_package(TclStub)
add_compile_definitions(USE_TCL_STUBS)

find_path(TCL_INCLUDE_DIR tcl.h REQUIRED
    HINTS
    ${CMAKE_SOURCE_DIR}/../tcl/generic
    ${CMAKE_SOURCE_DIR}/../tcl86/generic
    ${CMAKE_SOURCE_DIR}/../tcl87/generic
    /usr/local/tcl86/include /usr/local/tcl87/include /usr/include/tcl8.6)

target_include_directories(HttpParser
  PRIVATE
  "${CMAKE_CURRENT_BINARY_DIR}"
  "${TCL_INCLUDE_PATH}"
  "${CMAKE_SOURCE_DIR}/include")

target_link_libraries(HttpParser
    PRIVATE
    ${TCL_STUB_LIBRARY})

# Copy TCL package index
add_custom_target(copy_pkg_index SOURCES
    "http_parser/pkgIndex.tcl"
    COMMAND
    ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/http_parser
        ${CMAKE_CURRENT_BINARY_DIR}/http_parser)

# Copy test folder to binary directory
add_custom_target(copy_tcl_tests SOURCES
    "tests/all.tcl"
    "tests/parser.test"
    "tests/server.test"
    COMMAND
    ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/tests
        ${CMAKE_CURRENT_BINARY_DIR}/tests)

# Copy examples folder to binary directory
add_custom_target(copy_examples SOURCES
    "examples/http_server.tcl"
    COMMAND
    ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/examples
        ${CMAKE_CURRENT_BINARY_DIR}/examples)

add_dependencies(HttpParser copy_pkg_index copy_tcl_tests copy_examples)

# copy dll into http_parser package directory
add_custom_command(TARGET HttpParser POST_BUILD
    COMMAND
    ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:HttpParser>
        ${CMAKE_CURRENT_BINARY_DIR}/http_parser)

#add_custom_target(
#  TclTest ALL
#  COMMAND ls -la && cd tests && tclsh all.tcl
#  COMMENT "run TCL tests")

