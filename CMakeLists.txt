PROJECT(MLIB)
include_directories(include)
AUX_SOURCE_DIRECTORY(src MLIB_SRC)
execute_process(
	COMMAND git describe --match=NeVeRmAtCh --always --abbrev=40 --dirty
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	OUTPUT_VARIABLE GIT_SHA1
	OUTPUT_STRIP_TRAILING_WHITESPACE
	)
execute_process(
	COMMAND git log -1 --format=%ad --date=local 
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	OUTPUT_VARIABLE GIT_DATE
	OUTPUT_STRIP_TRAILING_WHITESPACE
	)
execute_process(
	COMMAND git log -1 --format=%s 
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	OUTPUT_VARIABLE GIT_COMMIT_SUBJECT
	OUTPUT_STRIP_TRAILING_WHITESPACE
	)
add_definitions("-DGIT_SHA1=${GIT_SHA1}")
add_definitions("-DGIT_DATE=${GIT_DATE}")
add_definitions("-DGIT_COMMIT_SUBJECT=${GIT_COMMIT_SUBJECT}")
add_definitions(-DHAVE_PTHREADS)
string(LENGTH "${CMAKE_SOURCE_DIR}/" SOURCE_PATH_SIZE)
add_definitions("-DSOURCE_PATH_SIZE=${SOURCE_PATH_SIZE}")
configure_file(
	${CMAKE_SOURCE_DIR}/include/Version.h
	#${CMAKE_BINARY_DIR}/generated/Version.h
	)
ADD_LIBRARY(mlib STATIC ${MLIB_SRC})
ADD_SUBDIRECTORY(test)
