cmake_minimum_required(VERSION 3.5)
project(Assignment_5)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES main.cpp hit.h material.h object3D.h ray.h scene_parser.h scene_parser.cpp
        sphere.h sphere.cpp vectors.h matrix.h matrix.cpp group.h camera.h orthographicCamera.h
        orthographicCamera.cpp image.h image.cpp light.h light.cpp perspectiveCamera.cpp
        perspectiveCamera.h plane.h  plane.cpp triangle.h triangle.cpp transform.h glCanvas.h
        glCanvas.cpp phongMaterial.cpp phongMaterial.h rayTracer.cpp rayTracer.h rayTree.cpp
        rayTree.h BoundingBox.cpp BoundingBox.h Grid.cpp Grid.h MarchingInfo.cpp MarchingInfo.h object3dvector.h)
add_executable(Assignment_5 ${SOURCE_FILES})
target_link_libraries(Assignment_5 libopengl32.a libglu32.a libfreeglut.a)