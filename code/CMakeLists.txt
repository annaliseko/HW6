cmake_minimum_required(VERSION 2.8)
project ( raytrace )

# report particulars ---------------------------------------------
message("\nBuilding for system:\t"   ${CMAKE_SYSTEM_NAME})
message("Build type: \t\t"           ${CMAKE_BUILD_TYPE})
message("Building with compiler:  "  ${CMAKE_CXX_COMPILER} "\n")

# add any additional include directories here -------------------
include_directories(
            ./
    )

set(HEADERS
    *.h         # all local .h files
)

set(SRCS
    box.cpp
    group.cpp
    material.cpp
    rayfileinstance.cpp
    torus.cpp
    camera.cpp
    image.cpp
    parse.cpp
    roots.cpp
    triangle.cpp
    cone.cpp
    light.cpp
    perlin.cpp
    shape.cpp
    control.cpp
    macro.cpp
    pointlight.cpp
    shapegroup.cpp
    cylinder.cpp
    macroinstance.cpp
    rayfile.cpp
    sphere.cpp
    directionallight.cpp
    main.cpp
    rayfile_render.cpp
    spotlight.cpp
)


# Ask CMake to find us OpenGL and GLUT: -----------------------------------
find_package(OpenGL)
find_package(GLUT)

# Add linker flags like this: -----------------------------------
set(CMAKE_CXX_FLAGS "-std=c++11")

# Halt on errors
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wfatal-errors -Wno-deprecated-declarations -Wno-deprecated")

add_executable( ${PROJECT_NAME} ${SRCS})

# link the OpenGL/GLUT libraries to our executable
target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} )