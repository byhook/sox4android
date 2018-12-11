
cmake_minimum_required(VERSION 3.4.1)

add_library(native-sox
           SHARED
           src/main/cpp/native_sox.cpp
           src/main/cpp/audio_effect.cpp)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
set(CMAKE_VERBOSE_MAKEFILE on)

add_library(lib-sox
           STATIC
           IMPORTED)

set_target_properties(lib-sox
                    PROPERTIES IMPORTED_LOCATION
                    ${CMAKE_SOURCE_DIR}/libs/${ANDROID_ABI}/libsox.a
                    )

#头文件
include_directories(${CMAKE_SOURCE_DIR}/src/main/cpp/include)

target_link_libraries(native-sox
                    lib-sox
                    log)