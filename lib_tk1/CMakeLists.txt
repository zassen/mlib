cmake_minimum_required(VERSION 2.8)
PROJECT(MLIB_TK1)
set(CMAKE_C_COMPILER "arm-linux-gnueabihf-gcc")
set(CMAKE_CXX_COMPILER "arm-linux-gnueabihf-g++")
set(CMAKE_BUILD_TYPE Debug)
set(_VERSION_PATH "${MLIB_SOURCE_DIR}/include")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${MLIB_LIBRARY_OUTPUT_DIRECTORY} )

add_custom_target(mlibversion_tk1
	COMMAND cmake
	ARGS -DVERSION_PATH=${_VERSION_PATH} -P ${MLIB_SOURCE_DIR}/mlibversion.cmake
	COMMENT "SET MLIB VERSION"
	)
add_definitions(-DHAVE_PTHREADS)
string(LENGTH "${MLIB_SOURCE_DIR}/" SOURCE_PATH_SIZE)
message(STATUS "source_path_size:${SOURCE_PATH_SIZE}")
add_definitions("-DSOURCE_PATH_SIZE=${SOURCE_PATH_SIZE}")
ADD_LIBRARY(mlib_tk1 STATIC ${MLIB_SRC})
add_dependencies(mlib_tk1 mlibversion_tk1)
#ADD_SUBDIRECTORY(../test)
