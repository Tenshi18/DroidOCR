cmake_minimum_required(VERSION 3.22.1)

project(droidocr)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(NCNN_DIR ${CMAKE_SOURCE_DIR}/../jniLibs/ncnn)
set(OpenCV_DIR ${CMAKE_SOURCE_DIR}/../jniLibs/opencv-mobile-4.12.0-android/sdk/native/jni)

if(NOT EXISTS ${NCNN_DIR})
    message(FATAL_ERROR "NCNN not found at ${NCNN_DIR}. Please download ncnn-YYYYMMDD-android-vulkan.zip and extract to app/src/main/jniLibs/ncnn")
else()
    set(ncnn_DIR ${NCNN_DIR}/${ANDROID_ABI}/lib/cmake/ncnn)
    find_package(ncnn REQUIRED)
endif()

if(NOT EXISTS ${OpenCV_DIR})
    message(FATAL_ERROR "OpenCV not found at ${OpenCV_DIR}. Please download opencv-mobile-4.12.0-android.zip and extract to app/src/main/jniLibs/")
else()
    find_package(OpenCV REQUIRED core imgproc)
endif()

set(SOURCE_FILES
    droidocr_jni_full.cpp
    ppocrv5_full.cpp
)

add_library(droidocr SHARED ${SOURCE_FILES})

target_include_directories(droidocr PRIVATE
    ${CMAKE_SOURCE_DIR}
)

target_link_libraries(droidocr
    ncnn
    ${OpenCV_LIBS}
    android
    log
    jnigraphics
)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(droidocr PRIVATE -O3 -fvisibility=hidden)
endif()

