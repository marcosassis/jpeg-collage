cmake_minimum_required(VERSION 3.14)
project(jpeg-collage VERSION 0.0.1 LANGUAGES C)
include(FetchContent)

FetchContent_Declare(mozjpeg
    GIT_REPOSITORY  https://github.com/mozilla/mozjpeg

    SOURCE_DIR      ${CMAKE_SOURCE_DIR}/libjpeg/mozjpeg/src
    BINARY_DIR      ${CMAKE_SOURCE_DIR}/libjpeg/mozjpeg/build

    # CMAKE_ARGS      -DWITH_JPEG8=1 -DENABLE_SHARED=0

    # TMP_DIR      = <prefix>/tmp
    # STAMP_DIR    = <prefix>/src/<name>-stamp
    # DOWNLOAD_DIR = <prefix>/src
    # SOURCE_DIR   = <prefix>/src/<name>
    # BINARY_DIR   = <prefix>/src/<name>-build
    # INSTALL_DIR  = <prefix>
    # LOG_DIR      = <STAMP_DIR>

    # PREFIX ${CMAKE_SOURCE_DIR}/libjpeg/mozjpeg
)

# FetchContent_MakeAvailable(mozjpeg)

FetchContent_GetProperties(mozjpeg)
if(NOT mozjpeg_POPULATED)
    FetchContent_Populate(mozjpeg
        # SUBBUILD_DIR    ${CMAKE_SOURCE_DIR}/libjpeg/mozjpeg/sub-build
        # SOURCE_DIR      ${CMAKE_SOURCE_DIR}/libjpeg/mozjpeg/src
        # BINARY_DIR      ${CMAKE_SOURCE_DIR}/libjpeg/mozjpeg/build
    )
    
    set(WITH_JPEG8 ON CACHE INTERNAL "Bla1")
    set(ENABLE_SHARED OFF CACHE INTERNAL "Bla2")

    add_subdirectory(${mozjpeg_SOURCE_DIR} ${mozjpeg_BINARY_DIR})
endif()



# target_compile_options(jpeg
#     PUBLIC
#         WITH_JPEG8=ON
#         ENABLE_SHARED=OFF)




add_executable( jpeg-collage
                main.c 
                libjpeg_wrapper_exif.c
                libjpeg_wrapper_markers.c
                libjpeg_wrapper.c)

FetchContent_GetProperties(mozjpeg)
include_directories(jpeg-collage PUBLIC ${mozjpeg_SOURCE_DIR})
include_directories(jpeg-collage PUBLIC ${mozjpeg_BINARY_DIR})
target_link_directories(jpeg-collage PUBLIC ${mozjpeg_SOURCE_DIR} ${mozjpeg_BINARY_DIR})
target_link_libraries(jpeg-collage PUBLIC jpeg m)