cmake_minimum_required(VERSION 3.28)
project(MConverter)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)
file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_LIST_DIR}/common/include/*.h)



add_executable(MConverter src/main.cpp ${SOURCES} ${HEADERS})

file(REMOVE_RECURSE ${CMAKE_BINARY_DIR}/resources/data)

file(GLOB CASE_FILES "${CMAKE_SOURCE_DIR}/resources/data/*.m")
file(COPY ${CASE_FILES} DESTINATION ${CMAKE_BINARY_DIR}/resources/data)


target_include_directories(MConverter
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/common/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
