#-------------------------------------------------------------------------------
#	CMAKE SETTINGS
#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 2.8.12)

project("recoverer")

set(SOURCES
	Files.cpp
	Parser.cpp
	)
	
set(HEADERS
	Files.h
	OrderBook.h
	Parser.h
	)

add_executable(recoverer recoverer.cpp)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT recoverer)

add_library(lib STATIC ${HEADERS} ${SOURCES})

target_link_libraries(recoverer lib)