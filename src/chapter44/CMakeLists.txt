file(GLOB_RECURSE CHAPTER_44_FILES
    ${CMAKE_SOURCE_DIR}/vendor/glad/src/gl.c
    ${CMAKE_SOURCE_DIR}/include/common/glsl_program.h
    ${CMAKE_SOURCE_DIR}/src/common/glsl_program.cpp
    ${CMAKE_SOURCE_DIR}/include/common/bounding_box.h
    ${CMAKE_SOURCE_DIR}/src/common/bounding_box.cpp
    ${CMAKE_SOURCE_DIR}/include/common/obj_mesh.h
    ${CMAKE_SOURCE_DIR}/src/common/obj_mesh.cpp
    ${CMAKE_SOURCE_DIR}/src/chapter44/*.cpp)

add_executable(Chapter44 ${CHAPTER_44_FILES})

target_include_directories(Chapter44 PRIVATE "${CMAKE_SOURCE_DIR}/include")
target_include_directories(Chapter44 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/glfw/include")
target_include_directories(Chapter44 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/glad/include")
target_include_directories(Chapter44 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/glm")
target_include_directories(Chapter44 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/stb")

target_link_libraries(Chapter44 glfw)
target_link_libraries(Chapter44 glm)

set_target_properties(Chapter44 PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/Chapter44")