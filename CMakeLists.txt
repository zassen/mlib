cmake_minimum_required(VERSION 2.8)
PROJECT(MLIB)
exec_program(
	"git"
	ARGS describe --tags --always --dirty
	OUTPUT_VARIABLE TAGS
	RETURN_VALUE ret
	)
exec_program(
	"date"
	ARGS +%Y%m%d%H%M%S
	OUTPUT_VARIABLE BUILD_TIME 
	RETURN_VALUE ret
	)
#configure_file(
#	${CMAKE_SOURCE_DIR}/include/Version.h.in
#	${CMAKE_SOURCE_DIR}/include/Version.h
#	)
message(STATUS "variable TAGS:${TAGS}")
message(STATUS "variable BUILD_TIME:${BUILD_TIME}")
add_definitions(-DHAVE_PTHREADS)
string(LENGTH "${CMAKE_SOURCE_DIR}/" SOURCE_PATH_SIZE)
message(STATUS "source_path_size:${SOURCE_PATH_SIZE}")
add_definitions("-DSOURCE_PATH_SIZE=${SOURCE_PATH_SIZE}")
add_definitions(-D_TAGS="${TAGS}")
add_definitions(-D_BUILD_TIME="${BUILD_TIME}")
AUX_SOURCE_DIRECTORY(src MLIB_SRC)
include_directories(./include)
ADD_LIBRARY(mlib STATIC ${MLIB_SRC})
ADD_SUBDIRECTORY(test)
