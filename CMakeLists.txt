cmake_minimum_required(VERSION 2.8)
PROJECT(MLIB)
execute_process(
	COMMAND git describe --tags --always --dirty
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	OUTPUT_VARIABLE GIT_TAG
	ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE
	)
execute_process(
	COMMAND git log -1 --format=%ad --date=local 
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	OUTPUT_VARIABLE GIT_DATE
	ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE
	)
execute_process(
	COMMAND git log -1 --format=%s 
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	OUTPUT_VARIABLE GIT_COMMIT_SUBJECT
	ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE
	)
configure_file(
	${CMAKE_SOURCE_DIR}/include/Version.h.in
	${CMAKE_SOURCE_DIR}/include/Version.h
	)
message(STATUS "variable GIT_SHA1:${GIT_SHA1}")
add_definitions(-DHAVE_PTHREADS)
string(LENGTH "${CMAKE_SOURCE_DIR}/" SOURCE_PATH_SIZE)
message(STATUS "source_path_size:${SOURCE_PATH_SIZE}")
add_definitions("-DSOURCE_PATH_SIZE=${SOURCE_PATH_SIZE}")
add_definitions(-DGIT_TAG="${GIT_TAG}")
add_definitions(-DGIT_DATE="${GIT_DATE}")
add_definitions(-DGIT_COMMIT_SUBJECT="${GIT_COMMIT_SUBJECT}")
AUX_SOURCE_DIRECTORY(src MLIB_SRC)
include_directories(./include)
ADD_LIBRARY(mlib STATIC ${MLIB_SRC})
ADD_SUBDIRECTORY(test)
