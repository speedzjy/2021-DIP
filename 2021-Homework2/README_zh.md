# Homework2

大部分源码来自于 https://github.com/SouthEastUniversityLinuxClub/ImageWarping

本项目以CMake重新构建，并解决了源代码在应用一些图像变形情况下的向量溢出问题。

build_demo 文件夹提供了 windows 平台下的 demo。

## 依赖
Qt5

## 安装

### 1. Open .pro project

用 Qt creator 打开 .pro文件

### 2. CMake

#### Windows
在Qt下或VS(Visual Studio)以构建CMake工程的方式构建此项目。

#### Linux
~~~
mkdir build
cd build
cmake ..
make
~~~

