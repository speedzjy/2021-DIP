cmake_minimum_required(VERSION 3.5)

project(color_transfer)

set( CMAKE_CXX_FLAGS "-std=c++11")

find_package( OpenCV REQUIRED)
include_directories( ${OpenCV_INCLUDE_DIRS})

add_executable(color_transfer_use_lib src/use_lib.cpp)
target_link_libraries(color_transfer_use_lib ${OpenCV_LIBS} )


add_library(color_transfer 
    SHARED 
        src/ColorTransfer.cpp
)

target_include_directories(color_transfer 
    PUBLIC 
        ${PROJECT_SOURCE_DIR}/include
        ${OpenCV_INCLUDE_DIRS}
)

add_executable(color_transfer_use_formula src/use_formula.cpp)
target_link_libraries(color_transfer_use_formula 
    PRIVATE
    color_transfer
    ${OpenCV_LIBS}
)
