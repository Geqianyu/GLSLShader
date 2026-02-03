file(GLOB_RECURSE CHAPTER_46_FILES
    ${CMAKE_SOURCE_DIR}/vendor/glad/src/gl.c
    ${CMAKE_SOURCE_DIR}/include/common/glsl_program.h
    ${CMAKE_SOURCE_DIR}/src/common/glsl_program.cpp
    ${CMAKE_SOURCE_DIR}/src/chapter46/*.cpp)

add_executable(Chapter46 ${CHAPTER_46_FILES})

target_include_directories(Chapter46 PRIVATE "${CMAKE_SOURCE_DIR}/include")
target_include_directories(Chapter46 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/glfw/include")
target_include_directories(Chapter46 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/glad/include")
target_include_directories(Chapter46 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/glm")
target_include_directories(Chapter46 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/stb")

target_link_libraries(Chapter46 glfw)
target_link_libraries(Chapter46 glm)

set_target_properties(Chapter46 PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/Chapter46")