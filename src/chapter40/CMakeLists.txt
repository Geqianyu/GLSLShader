file(GLOB_RECURSE CHAPTER_40_FILES
    ${CMAKE_SOURCE_DIR}/vendor/glad/src/gl.c
    ${CMAKE_SOURCE_DIR}/include/common/glsl_program.h
    ${CMAKE_SOURCE_DIR}/src/common/glsl_program.cpp
    ${CMAKE_SOURCE_DIR}/include/common/triangle_mesh.h
    ${CMAKE_SOURCE_DIR}/src/common/triangle_mesh.cpp
    ${CMAKE_SOURCE_DIR}/include/common/plane.h
    ${CMAKE_SOURCE_DIR}/src/common/plane.cpp
    ${CMAKE_SOURCE_DIR}/include/common/teapot_data.h
    ${CMAKE_SOURCE_DIR}/include/common/teapot.h
    ${CMAKE_SOURCE_DIR}/src/common/teapot.cpp
    ${CMAKE_SOURCE_DIR}/include/common/torus.h
    ${CMAKE_SOURCE_DIR}/src/common/torus.cpp
    ${CMAKE_SOURCE_DIR}/src/chapter40/*.cpp)

add_executable(Chapter40 ${CHAPTER_40_FILES})

target_include_directories(Chapter40 PRIVATE "${CMAKE_SOURCE_DIR}/include")
target_include_directories(Chapter40 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/glfw/include")
target_include_directories(Chapter40 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/glad/include")
target_include_directories(Chapter40 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/glm")
target_include_directories(Chapter40 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/stb")

target_link_libraries(Chapter40 glfw)
target_link_libraries(Chapter40 glm)

set_target_properties(Chapter40 PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/Chapter40")