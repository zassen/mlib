cmake_minimum_required(VERSION 2.8)
PROJECT(MLIB)
set(_VERSION_PATH "${MLIB_SOURCE_DIR}/include")
set(MLIB_BINARY_DIR "${MLIB_SOURCE_DIR}/out")
set(MLIB_LIBRARY_OUTPUT_DIRECTORY "${MLIB_SOURCE_DIR}/out/")
#add_custom_target(mlibversion
#	COMMAND cmake
#	ARGS -DVERSION_PATH=${_VERSION_PATH} -P ${MLIB_SOURCE_DIR}/mlibversion.cmake
#	COMMENT "SET MLIB VERSION"
#	)
add_definitions(-DHAVE_PTHREADS)
string(LENGTH "${MLIB_SOURCE_DIR}/" SOURCE_PATH_SIZE)
message(STATUS "source_path_size:${SOURCE_PATH_SIZE}")
add_definitions("-DSOURCE_PATH_SIZE=${SOURCE_PATH_SIZE}")
AUX_SOURCE_DIRECTORY(${MLIB_SOURCE_DIR}/src MLIB_SRC)
include_directories(${MLIB_SOURCE_DIR}/include)
#ADD_LIBRARY(mlib STATIC ${MLIB_SRC})
#add_dependencies(mlib mlibversion)
ADD_SUBDIRECTORY(test)
ADD_SUBDIRECTORY(lib_pc)
ADD_SUBDIRECTORY(lib_tk1)
ADD_SUBDIRECTORY(lib_linaro)
