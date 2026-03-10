file(GLOB_RECURSE CHAPTER_49_FILES
    ${CMAKE_SOURCE_DIR}/vendor/glad/src/gl.c
    ${CMAKE_SOURCE_DIR}/include/common/glsl_program.h
    ${CMAKE_SOURCE_DIR}/src/common/glsl_program.cpp
    ${CMAKE_SOURCE_DIR}/include/common/teapot_data.h
    ${CMAKE_SOURCE_DIR}/include/common/teapot_patch.h
    ${CMAKE_SOURCE_DIR}/src/common/teapot_patch.cpp
    ${CMAKE_SOURCE_DIR}/src/chapter49/*.cpp)

add_executable(Chapter49 ${CHAPTER_49_FILES})

target_include_directories(Chapter49 PRIVATE "${CMAKE_SOURCE_DIR}/include")
target_include_directories(Chapter49 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/glfw/include")
target_include_directories(Chapter49 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/glad/include")
target_include_directories(Chapter49 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/glm")
target_include_directories(Chapter49 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/stb")

target_link_libraries(Chapter49 glfw)
target_link_libraries(Chapter49 glm)

set_target_properties(Chapter49 PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/Chapter49")