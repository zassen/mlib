cmake_minimum_required(VERSION 2.8)
PROJECT(MLIB_PC)
set(_VERSION_PATH "${MLIB_SOURCE_DIR}/include")
set(MLIB_PC_BINARY_DIR ${MLIB_BINARY_DIR} FORCE)
message(BINARY DIR "${MLIB_BINARY_DIR}")
set(MLIB_PC_LIBRARY_OUTPUT_DIRECTORY ${MLIB_LIBRARY_OUTPUT_DIRECTORY} )

add_custom_target(mlibversion_pc
	COMMAND cmake
	ARGS -DVERSION_PATH=${_VERSION_PATH} -P ${MLIB_SOURCE_DIR}/mlibversion.cmake
	COMMENT "SET MLIB VERSION"
	)
add_definitions(-DHAVE_PTHREADS)
string(LENGTH "${MLIB_SOURCE_DIR}/" SOURCE_PATH_SIZE)
message(STATUS "source_path_size:${SOURCE_PATH_SIZE}")
add_definitions("-DSOURCE_PATH_SIZE=${SOURCE_PATH_SIZE}")
ADD_LIBRARY(mlib_pc STATIC ${MLIB_SRC})
add_dependencies(mlib_pc mlibversion_pc)
#ADD_SUBDIRECTORY(../test)
