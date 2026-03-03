file(GLOB_RECURSE CHAPTER_47_FILES
    ${CMAKE_SOURCE_DIR}/vendor/glad/src/gl.c
    ${CMAKE_SOURCE_DIR}/include/common/glsl_program.h
    ${CMAKE_SOURCE_DIR}/src/common/glsl_program.cpp
    ${CMAKE_SOURCE_DIR}/src/chapter47/*.cpp)

add_executable(Chapter47 ${CHAPTER_47_FILES})

target_include_directories(Chapter47 PRIVATE "${CMAKE_SOURCE_DIR}/include")
target_include_directories(Chapter47 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/glfw/include")
target_include_directories(Chapter47 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/glad/include")
target_include_directories(Chapter47 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/glm")
target_include_directories(Chapter47 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/stb")

target_link_libraries(Chapter47 glfw)
target_link_libraries(Chapter47 glm)

set_target_properties(Chapter47 PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/Chapter47")