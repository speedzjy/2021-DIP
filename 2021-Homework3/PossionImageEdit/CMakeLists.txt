cmake_minimum_required(VERSION 2.8)

project(PossionImageEdit)

set(CMAKE_BUILD_TYPE  Debug) #设置编译类型为debug

set(CMAKE_CXX_STANDARD 11) #设置C++版本为C11
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON) #设置当前包含的头文件开启


set(EIGEN_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/eigen)

include_directories(
  ${PROJECT_SOURCE_DIR}/src,
  ${EIGEN_INCLUDE_DIRS}
)

set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/possionEdit.cpp
    src/possionEdit.h
)

find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

qt5_wrap_cpp(MAIN_WINDOW_SRCS
    src/mainwindow.h
)
qt5_wrap_ui(MAIN_WINDOW_UI
    src/mainwindow.ui)

add_executable(ImageWraping
    ${SOURCES}
    ${MAIN_WINDOW_UI}
    ${MAIN_WINDOW_SRCS})

target_link_libraries(ImageWraping Qt5::Core Qt5::Widgets Qt5::Gui)




