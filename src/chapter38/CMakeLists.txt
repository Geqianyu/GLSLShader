file(GLOB_RECURSE CHAPTER_38_FILES
    ${CMAKE_SOURCE_DIR}/vendor/glad/src/gl.c
    ${CMAKE_SOURCE_DIR}/include/common/glsl_program.h
    ${CMAKE_SOURCE_DIR}/src/common/glsl_program.cpp
    ${CMAKE_SOURCE_DIR}/include/common/triangle_mesh.h
    ${CMAKE_SOURCE_DIR}/src/common/triangle_mesh.cpp
    ${CMAKE_SOURCE_DIR}/include/common/bounding_box.h
    ${CMAKE_SOURCE_DIR}/src/common/bounding_box.cpp
    ${CMAKE_SOURCE_DIR}/include/common/obj_mesh.h
    ${CMAKE_SOURCE_DIR}/src/common/obj_mesh.cpp
    ${CMAKE_SOURCE_DIR}/src/chapter38/*.cpp)

add_executable(Chapter38 ${CHAPTER_38_FILES})

target_include_directories(Chapter38 PRIVATE "${CMAKE_SOURCE_DIR}/include")
target_include_directories(Chapter38 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/glfw/include")
target_include_directories(Chapter38 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/glad/include")
target_include_directories(Chapter38 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/glm")
target_include_directories(Chapter38 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/stb")

target_link_libraries(Chapter38 glfw)
target_link_libraries(Chapter38 glm)

set_target_properties(Chapter38 PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/Chapter38")