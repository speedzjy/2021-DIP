cmake_minimum_required(VERSION 2.8)

project(ImageWraping)

set(CMAKE_BUILD_TYPE  Debug) #设置编译类型为debug

set(CMAKE_CXX_STANDARD 11) #设置C++版本为C11
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON) #设置当前包含的头文件开启

set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/idw.cpp
    src/rbf.cpp
    src/mainscene.cpp
    src/mainview.cpp
    src/idw.h
    src/rbf.h
)

find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

qt5_wrap_cpp(MAIN_WINDOW_SRCS
    src/mainwindow.h
    src/mainview.h
    src/mainscene.h
)
qt5_wrap_ui(MAIN_WINDOW_UI
    src/mainwindow.ui)

add_executable(ImageWraping
    ${SOURCES}
    ${MAIN_WINDOW_UI}
    ${MAIN_WINDOW_SRCS})

target_link_libraries(ImageWraping Qt5::Core Qt5::Widgets Qt5::Gui)




