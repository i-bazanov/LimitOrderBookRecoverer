#-------------------------------------------------------------------------------
#	CMAKE SETTINGS
#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.18.0)

project("recoverer")

include(ExternalProject)
include("${CMAKE_SOURCE_DIR}/rapidJson/rapidjson.cmake")

include_directories(${RAPIDJSON_INCLUDE_DIR})

set(SOURCES
	Files.cpp
	Parser.cpp
	UpdateTime.cpp
	)
	
set(HEADERS
	Files.h
	OrderBook.h
	Parser.h
	UpdateTime.h
	)

add_executable(${PROJECT_NAME} recoverer.cpp)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

add_library(lib STATIC ${HEADERS} ${SOURCES})
add_dependencies(${PROJECT_NAME} rapidjson)

target_link_libraries(${PROJECT_NAME} lib)
