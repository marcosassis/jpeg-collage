include(FetchContent)

FetchContent_Declare(mozjpeg
    GIT_REPOSITORY  https://github.com/mozilla/mozjpeg
    SOURCE_DIR      ${CMAKE_SOURCE_DIR}/libjpeg/mozjpeg
    BINARY_DIR      ${CMAKE_SOURCE_DIR}/libjpeg/mozjpeg-build
    SUBBUILD_DIR    ${CMAKE_SOURCE_DIR}/libjpeg/mozjpeg-subbuild
)

FetchContent_GetProperties(mozjpeg)
if(NOT mozjpeg_POPULATED)
    FetchContent_Populate(mozjpeg)
    set(WITH_JPEG8 ON CACHE INTERNAL "")
    set(ENABLE_SHARED OFF CACHE INTERNAL "")
    add_subdirectory(${mozjpeg_SOURCE_DIR} ${mozjpeg_BINARY_DIR})
endif()

add_library(libjpeg_wrapper STATIC libjpeg_wrapper.c libjpeg_wrapper_exif.c libjpeg_wrapper_markers.c)
target_include_directories(libjpeg_wrapper PUBLIC ${mozjpeg_SOURCE_DIR} ${mozjpeg_BINARY_DIR})
target_link_directories(libjpeg_wrapper PUBLIC ${mozjpeg_SOURCE_DIR} ${mozjpeg_BINARY_DIR})
target_link_libraries(libjpeg_wrapper PUBLIC jpeg-static m)
