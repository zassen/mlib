cmake_minimum_required(VERSION 2.8)
PROJECT(MLIB_LINARO)
set(_VERSION_PATH "${MLIB_SOURCE_DIR}/include")

add_custom_target(mlibversion_linaro
	COMMAND cmake
	ARGS -DVERSION_PATH=${_VERSION_PATH} -P ${MLIB_SOURCE_DIR}/mlibversion.cmake
	COMMENT "SET MLIB VERSION"
	)
add_definitions(-DHAVE_PTHREADS)
string(LENGTH "${MLIB_SOURCE_DIR}/" SOURCE_PATH_SIZE)
message(STATUS "source_path_size:${SOURCE_PATH_SIZE}")
add_definitions("-DSOURCE_PATH_SIZE=${SOURCE_PATH_SIZE}")
ADD_LIBRARY(mlib_linaro STATIC ${MLIB_SRC})
add_dependencies(mlib_linaro mlibversion_linaro)
#ADD_SUBDIRECTORY(../test)
