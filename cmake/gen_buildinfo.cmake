execute_process(
    COMMAND git describe --always --dirty --tags
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_DESCRIBE
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

file(WRITE ${CMAKE_BINARY_DIR}/generated/build_info.h
"#pragma once
#define BUILD_GIT \"${GIT_DESCRIBE}\"
")