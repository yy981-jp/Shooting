execute_process(
    COMMAND git describe --always --dirty --tags
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_DESCRIBE
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

string(TIMESTAMP BUILD_DATETIME "%Y-%m-%d %H:%M:%S")

file(WRITE ${CMAKE_SOURCE_DIR}/assets/build.info "${GIT_DESCRIBE},${BUILD_DATETIME}\n")