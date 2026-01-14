file(GLOB_RECURSE CHAPTER_06_FILES
    ${CMAKE_SOURCE_DIR}/vendor/glad/src/gl.c
    ${CMAKE_SOURCE_DIR}/include/common/glsl_program.h
    ${CMAKE_SOURCE_DIR}/src/common/glsl_program.cpp
    ${CMAKE_SOURCE_DIR}/src/chapter06/*.cpp)

add_executable(Chapter06 ${CHAPTER_06_FILES})

target_include_directories(Chapter06 PRIVATE "${CMAKE_SOURCE_DIR}/include")
target_include_directories(Chapter06 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/glfw/include")
target_include_directories(Chapter06 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/glad/include")
target_include_directories(Chapter06 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/glm")

target_link_libraries(Chapter06 glfw)
target_link_libraries(Chapter06 glm)

set_target_properties(Chapter06 PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/Chapter06")