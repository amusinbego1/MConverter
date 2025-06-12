cmake_minimum_required(VERSION 3.28)
set(SOLUTION_NAME MConverter)
project(${SOLUTION_NAME})

set(CMAKE_CXX_STANDARD 17)

set(SOURCE_ROOT ${CMAKE_CURRENT_LIST_DIR})
set(HOME_ROOT $ENV{HOME})

if (WIN32)
    string(REPLACE "\\" "/" HOME_ROOT "${HOME_ROOT}")
endif ()

set(WORK_ROOT ${HOME_ROOT}/Work)

include(${WORK_ROOT}/DevEnv/Common.cmake)
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)
file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_LIST_DIR}/common/include/*.h)
file(GLOB_RECURSE MCONVERTER_TD ${MY_INC}/td/*.h)
file(GLOB_RECURSE MCONVERTER_CNT ${MY_INC}/cnt/*.h)
file(GLOB_RECURSE MCONVERTER_MU ${MY_INC}/mu/*.h)
file(GLOB_RECURSE MCONVERTER_FO ${MY_INC}/fo/*.h)
file(GLOB_RECURSE MCONVERTER_XML ${MY_INC}/xml/*.h)


add_executable(${SOLUTION_NAME} src/main.cpp ${SOURCES} ${HEADERS} ${MCONVERTER_TD} ${MCONVERTER_CNT} ${MCONVERTER_MU} ${MCONVERTER_FO}
                                       ${MCONVERTER_XML})

target_link_libraries(${SOLUTION_NAME}
        debug ${MU_LIB_DEBUG} optimized ${MU_LIB_RELEASE}
)

file(REMOVE_RECURSE ${CMAKE_BINARY_DIR}/resources/data)
file(GLOB CASE_FILES "${CMAKE_SOURCE_DIR}/resources/data/*.m")
file(COPY ${CASE_FILES} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}/resources/data)

target_compile_definitions(${SOLUTION_NAME} PRIVATE OUTPUT_PATH="${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}/")


target_include_directories(${SOLUTION_NAME}
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/common/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
