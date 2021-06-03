cmake_minimum_required(VERSION 3.14)
project(jpeg-collage VERSION 0.0.1 LANGUAGES C)

add_subdirectory(libjpeg_wrapper)
add_executable(jpeg-collage main.c)
target_include_directories(libjpeg_wrapper PUBLIC ${CMAKE_SOURCE_DIR}/libjpeg_wrapper)
target_link_libraries(jpeg-collage PUBLIC libjpeg_wrapper)
