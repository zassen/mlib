PROJECT(MLIB)
include_directories(include)
#AUX_SOURCE_DIRECTORY(src MLIB_SRC)
execute_process(
	COMMAND git describe --tags --dirty
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	OUTPUT_VARIABLE GIT_SHA1
	ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE
	)
#exec_program(
#	"git"
#	${CMAKE_SOURCE_DIR}
#	ARGS "git describe --match=NeVeRmAtCh --always --abbrev=40 --dirty"
#	OUTPUT_VARIABLE GIT_SHA1)
#exec_program(
#	"git"
#	${CMAKE_SOURCE_DIR}
#	ARGS " git log -1 --format=%ad --date=local "
#	OUTPUT_VARIABLE GIT_DATE)
#exec_program(
#	"git"
#	${CMAKE_SOURCE_DIR}
#	ARGS " git log -1 --format=%s"
#	OUTPUT_VARIABLE GIT_COMMIT_SUBJECT)
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
#add_definitions("-DGIT_SHA1=${GIT_SHA1}")
#add_definitions("-DGIT_DATE=${GIT_DATE}")
#add_definitions("-DGIT_COMMIT_SUBJECT=${GIT_COMMIT_SUBJECT}")
message(STATUS "variable GIT_SHA1:${GIT_SHA1}")
add_definitions(-DHAVE_PTHREADS)
string(LENGTH "${CMAKE_SOURCE_DIR}/" SOURCE_PATH_SIZE)
message(STATUS "source_path_size:${SOURCE_PATH_SIZE}")
add_definitions(-DSOURCE_PATH_SIZE="${SOURCE_PATH_SIZE}")
#include_directories(${CMAKE_BINARY_DIR}/generated)
add_definitions(-DGIT_SHA1="${GIT_SHA1}")
add_definitions(-DGIT_DATE="${GIT_DATE}")
add_definitions(-DGIT_COMMIT_SUBJECT="${GIT_COMMIT_SUBJECT}")
AUX_SOURCE_DIRECTORY(src MLIB_SRC)
ADD_LIBRARY(mlib STATIC ${MLIB_SRC})
ADD_SUBDIRECTORY(test)
