file(GLOB_RECURSE CHAPTER_32_FILES
    ${CMAKE_SOURCE_DIR}/vendor/glad/src/gl.c
    ${CMAKE_SOURCE_DIR}/include/common/glsl_program.h
    ${CMAKE_SOURCE_DIR}/src/common/glsl_program.cpp
    ${CMAKE_SOURCE_DIR}/include/common/triangle_mesh.h
    ${CMAKE_SOURCE_DIR}/src/common/triangle_mesh.cpp
    ${CMAKE_SOURCE_DIR}/include/common/plane.h
    ${CMAKE_SOURCE_DIR}/src/common/plane.cpp
    ${CMAKE_SOURCE_DIR}/include/common/texture.h
    ${CMAKE_SOURCE_DIR}/src/common/texture.cpp
    ${CMAKE_SOURCE_DIR}/src/chapter32/*.cpp)

add_executable(Chapter32 ${CHAPTER_32_FILES})

target_include_directories(Chapter32 PRIVATE "${CMAKE_SOURCE_DIR}/include")
target_include_directories(Chapter32 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/glfw/include")
target_include_directories(Chapter32 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/glad/include")
target_include_directories(Chapter32 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/glm")
target_include_directories(Chapter32 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/stb")

target_link_libraries(Chapter32 glfw)
target_link_libraries(Chapter32 glm)

set_target_properties(Chapter32 PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/Chapter32")