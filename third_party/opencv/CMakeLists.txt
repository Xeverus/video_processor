cmake_minimum_required(VERSION 3.23)

include(ExternalProject)

ExternalProject_Add(
        "opencv"
        SOURCE_DIR "${CMAKE_SOURCE_DIR}/third_party/opencv/opencv/"
        BINARY_DIR "${CMAKE_BINARY_DIR}/third_party/opencv/bin/"
        CMAKE_ARGS
            -DBUILD_JAVA=OFF
            -DBUILD_FAT_JAVA_LIB=OFF
            -DBUILD_opencv_python2=OFF
            -DBUILD_opencv_python3=OFF
            -DBUILD_LIST=videoio
            -DBUILD_PROTOBUF=OFF
            -DBUILD_opencv_apps=OFF
            -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
            -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
            -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_BINARY_DIR}/install/opencv
            -DOPENCV_DNN_OPENCL=OFF
            -DWITH_OPENCL=OFF
            -DWITH_PNG=OFF
            -DWITH_JPEG=OFF
            -DWITH_TIFF=OFF
            -DWITH_WEBP=OFF
            -DWITH_OPENJPEG=OFF
            -DWITH_JASPER=OFF
            -DWITH_OPENEXR=OFF
            -DWITH_V4L=OFF
            -DWITH_ANDROID_MEDIANDK=OFF
            -DWITH_GTK=OFF
            -DWITH_WIN32UI=OFF
            -DWITH_PROTOBUF=OFF
            -DWITH_DSHOW=OFF
            -DWITH_IMGCODEC_HDR=OFF
            -DWITH_IMGCODEC_SUNRASTER=OFF
            -DWITH_IMGCODEC_PXM=OFF
            -DWITH_IMGCODEC_PFM=OFF
        TEST_COMMAND ""
)
set_property(TARGET "opencv" PROPERTY CXX_STANDARD ${CMAKE_CXX_STANDARD})
