cmake_minimum_required(VERSION 2.8)
PROJECT(MLIB)
#exec_program(
#	"git"
#	ARGS describe --tags --always --dirty
#	OUTPUT_VARIABLE TAGS
#	RETURN_VALUE ret
#	)
#exec_program(
#	"date"
#	ARGS +%Y%m%d%H%M%S
#	OUTPUT_VARIABLE BUILD_TIME 
#	RETURN_VALUE ret
#	)
#configure_file(
#	${MLIB_SOURCE_DIR}/include/Version.h.in
#	${MLIB_SOURCE_DIR}/include/Version.h
#	)
set(_VERSION_PATH "${MLIB_SOURCE_DIR}/include")
add_custom_target(mlibversion
	COMMAND cmake
	ARGS -DVERSION_PATH=${_VERSION_PATH} -P ${MLIB_SOURCE_DIR}/mlibversion.cmake
	COMMENT "SET MLIB VERSION"
	)
add_definitions(-DHAVE_PTHREADS)
string(LENGTH "${MLIB_SOURCE_DIR}/" SOURCE_PATH_SIZE)
message(STATUS "source_path_size:${SOURCE_PATH_SIZE}")
add_definitions("-DSOURCE_PATH_SIZE=${SOURCE_PATH_SIZE}")
AUX_SOURCE_DIRECTORY(${MLIB_SOURCE_DIR}/src MLIB_SRC)
include_directories(./include)
ADD_LIBRARY(mlib STATIC ${MLIB_SRC})
add_dependencies(mlib mlibversion)
ADD_SUBDIRECTORY(test)
ADD_SUBDIRECTORY(lib_pc)
