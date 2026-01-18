file(GLOB_RECURSE CHAPTER_15_FILES
    ${CMAKE_SOURCE_DIR}/vendor/glad/src/gl.c
    ${CMAKE_SOURCE_DIR}/include/common/glsl_program.h
    ${CMAKE_SOURCE_DIR}/src/common/glsl_program.cpp
    ${CMAKE_SOURCE_DIR}/include/common/triangle_mesh.h
    ${CMAKE_SOURCE_DIR}/src/common/triangle_mesh.cpp
    ${CMAKE_SOURCE_DIR}/include/common/torus.h
    ${CMAKE_SOURCE_DIR}/src/common/torus.cpp
    ${CMAKE_SOURCE_DIR}/src/chapter15/*.cpp)

add_executable(Chapter15 ${CHAPTER_15_FILES})

target_include_directories(Chapter15 PRIVATE "${CMAKE_SOURCE_DIR}/include")
target_include_directories(Chapter15 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/glfw/include")
target_include_directories(Chapter15 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/glad/include")
target_include_directories(Chapter15 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/glm")

target_link_libraries(Chapter15 glfw)
target_link_libraries(Chapter15 glm)

set_target_properties(Chapter15 PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/Chapter15")