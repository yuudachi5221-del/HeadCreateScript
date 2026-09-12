cmake_minimum_required(VERSION 3.15)
project(HeadCreateScript VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files for the HCS library
set(HCS_SOURCES
    src/lexer.cpp
    src/parser.cpp
    src/interpreter.cpp
    src/hcs_value.cpp
    src/runtime.cpp
)

# Create static library
add_library(hcs_lib STATIC ${HCS_SOURCES})
target_include_directories(hcs_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Create executable
add_executable(hcs src/main.cpp)
target_compile_definitions(hcs PRIVATE HCS_VERSION="${PROJECT_VERSION}")
target_link_libraries(hcs PRIVATE hcs_lib)

# Enable testing
enable_testing()

# Lexer tests
add_executable(lexer_test tests/lexer_test.cpp)
target_link_libraries(lexer_test PRIVATE hcs_lib)
add_test(NAME LexerTest COMMAND lexer_test)
