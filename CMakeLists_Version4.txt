cmake_minimum_required(VERSION 3.15)
project(HeadCreateScript VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Library sources
set(HCS_LIB_SOURCES
  src/lexer.cpp
  src/runtime.cpp
  src/launcher.cpp           # launcher をライブラリに含める（main 等から利用可能にする）
)

add_library(hcs_lib STATIC ${HCS_LIB_SOURCES})
target_include_directories(hcs_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_executable(hcs src/main.cpp)

# Provide the project version to the compiler so main.cpp can print it
target_compile_definitions(hcs PRIVATE HCS_VERSION=\"${PROJECT_VERSION}\")

target_link_libraries(hcs PRIVATE hcs_lib)

enable_testing()
add_executable(lexer_test tests/lexer_test.cpp)
target_link_libraries(lexer_test PRIVATE hcs_lib)
add_test(NAME LexerTest COMMAND lexer_test)