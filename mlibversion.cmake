execute_process(COMMAND
	git describe --tags --always --dirty
	OUTPUT_VARIABLE _TAGS
	ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND
	date +%Y%m%d%H%M%S
	OUTPUT_VARIABLE _BUILD_TIME
	ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
message(_TAGS:${_TAGS})
message(_BUILD_TIME:${_BUILD_TIME})
message(VERSION_PATH:${VERSION_PATH})
configure_file("${VERSION_PATH}/Version.h.in" "${VERSION_PATH}/Version.h")
