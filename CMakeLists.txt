cmake_minimum_required(VERSION 3.1)
project(final)

add_subdirectory(src/ext)

# include directories
include_directories(${PROJECT_SOURCE_DIR}/src
                    ${PROJECT_SOURCE_DIR}/src/ext
                    ${PROJECT_SOURCE_DIR}/src/ext/stb
                    ${PROJECT_SOURCE_DIR}/src/ext/eigen
        )

set(ext_stb_srcs
    src/ext/stb/stb_image_write.h
    src/ext/stb/stb_image.h
)

add_definitions(-DDATA_DIR=R\"\(${PROJECT_SOURCE_DIR}/data\)\")

set(common_srcs
    src/array3D.h
    src/exceptions.h
    src/floatimage.cpp
    src/floatimage.h
    src/utils.h
)

# Compile common library
add_library(common_lib STATIC
    ${common_srcs}
    ${ext_stb_srcs}
)
SOURCE_GROUP("src\\ext\\stb" FILES ${ext_stb_srcs})

set(final_srcs src/main.cpp src/filtering.cpp src/a2.cpp src/a6.cpp src/poisson.cpp src/poisson.cpp src/poisson.h)
add_executable(final ${final_srcs})
target_link_libraries(final common_lib)
SOURCE_GROUP("src" FILES ${final_srcs})


# Create the output data directory
add_custom_command(TARGET final POST_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_SOURCE_DIR}/data/output")