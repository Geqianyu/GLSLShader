file(GLOB_RECURSE CHAPTER_43_FILES
    ${CMAKE_SOURCE_DIR}/vendor/glad/src/gl.c
    ${CMAKE_SOURCE_DIR}/include/common/glsl_program.h
    ${CMAKE_SOURCE_DIR}/src/common/glsl_program.cpp
    ${CMAKE_SOURCE_DIR}/include/common/texture.h
    ${CMAKE_SOURCE_DIR}/src/common/texture.cpp
    ${CMAKE_SOURCE_DIR}/src/chapter43/*.cpp)

add_executable(Chapter43 ${CHAPTER_43_FILES})

target_include_directories(Chapter43 PRIVATE "${CMAKE_SOURCE_DIR}/include")
target_include_directories(Chapter43 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/glfw/include")
target_include_directories(Chapter43 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/glad/include")
target_include_directories(Chapter43 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/glm")
target_include_directories(Chapter43 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/stb")

target_link_libraries(Chapter43 glfw)
target_link_libraries(Chapter43 glm)

set_target_properties(Chapter43 PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/Chapter43")