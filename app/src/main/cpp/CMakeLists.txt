# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

#指定 cmake 的最小版本
cmake_minimum_required(VERSION 3.10.2)

# Declares and names the project.
#指定项目名称
project(tcp-lib)  #PROJECT_NAME

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

#指定头文件目录位置
include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/log)
include_directories(${PROJECT_SOURCE_DIR}/net)
include_directories(${PROJECT_SOURCE_DIR}/net/tcp/client)
include_directories(${PROJECT_SOURCE_DIR}/net/tcp/server)
include_directories(${PROJECT_SOURCE_DIR}/platform/android)
include_directories(${PROJECT_SOURCE_DIR}/util)

#指定源文件目录位置
aux_source_directory(. CPP_ROOT) #搜索当前目录下的所有cpp
aux_source_directory(./log CPP_LOG)  # /log搜索当前目录下的所有cpp
aux_source_directory(./net CPP_PDU)
aux_source_directory(./net/tcp/client CPP_NET_CLIENT)
aux_source_directory(./net/tcp/server CPP_NET_SERVER)
aux_source_directory(./platform/android CPP_ANDROID)


#add_executable(demo demo.cpp) # 生成可执行文件
#add_library(common STATIC util.cpp) # 生成静态库
#add_library(common SHARED util.cpp) # 生成动态库或共享库
#指定的源文件生成链接文件
add_library( # Sets the name of the library.
        ${PROJECT_NAME}

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        ${CPP_ROOT}
        ${CPP_LOG}
        ${CPP_PDU}
        ${CPP_NET_CLIENT}
        ${CPP_NET_SERVER}
        ${CPP_ANDROID}
        )

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

#find_library(VAR name path)查找到指定的预编译库，并将它的路径存储在变量中
#默认的搜索路径为 cmake 包含的系统库，因此如果是 NDK 的公共库只需要指定库的 name 即可
find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

#将目标文件与库文件进行链接
target_link_libraries( # Specifies the target library.
        ${PROJECT_NAME}

        # Links the target library to the log library
        # included in the NDK.
        ${log-lib})