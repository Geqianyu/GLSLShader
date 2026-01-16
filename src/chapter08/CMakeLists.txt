file(GLOB_RECURSE CHAPTER_08_FILES
    ${CMAKE_SOURCE_DIR}/vendor/glad/src/gl.c
    ${CMAKE_SOURCE_DIR}/include/common/glsl_program.h
    ${CMAKE_SOURCE_DIR}/src/common/glsl_program.cpp
    ${CMAKE_SOURCE_DIR}/include/common/triangle_mesh.h
    ${CMAKE_SOURCE_DIR}/src/common/triangle_mesh.cpp
    ${CMAKE_SOURCE_DIR}/include/common/torus.h
    ${CMAKE_SOURCE_DIR}/src/common/torus.cpp
    ${CMAKE_SOURCE_DIR}/src/chapter08/*.cpp)

add_executable(Chapter08 ${CHAPTER_08_FILES})

target_include_directories(Chapter08 PRIVATE "${CMAKE_SOURCE_DIR}/include")
target_include_directories(Chapter08 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/glfw/include")
target_include_directories(Chapter08 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/glad/include")
target_include_directories(Chapter08 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/glm")

target_link_libraries(Chapter08 glfw)
target_link_libraries(Chapter08 glm)

set_target_properties(Chapter08 PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/Chapter08")