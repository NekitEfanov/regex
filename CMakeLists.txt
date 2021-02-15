cmake_minimum_required(VERSION 3.5)

project(regex LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set( INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include )
set( SRC_DIR ${CMAKE_SOURCE_DIR}/src )

include_directories( ${INCLUDE_DIR} )

file( GLOB HEADERS "${INCLUDE_DIR}/*.h" "${INCLUDE_DIR}/*.hpp" )
file( GLOB SOURCES "${SRC_DIR}/*.c" "${SRC_DIR}/*.cpp" )

set( PROJECT_SOURCES ${SOURCES} ${HEADERS} )

add_executable(regex ${SOURCES} ${HEADERS} )