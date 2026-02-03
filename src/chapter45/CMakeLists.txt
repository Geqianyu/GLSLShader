file(GLOB_RECURSE CHAPTER_45_FILES
    ${CMAKE_SOURCE_DIR}/vendor/glad/src/gl.c
    ${CMAKE_SOURCE_DIR}/include/common/glsl_program.h
    ${CMAKE_SOURCE_DIR}/src/common/glsl_program.cpp
    ${CMAKE_SOURCE_DIR}/include/common/bounding_box.h
    ${CMAKE_SOURCE_DIR}/src/common/bounding_box.cpp
    ${CMAKE_SOURCE_DIR}/include/common/obj_mesh.h
    ${CMAKE_SOURCE_DIR}/src/common/obj_mesh.cpp
    ${CMAKE_SOURCE_DIR}/src/chapter45/*.cpp)

add_executable(Chapter45 ${CHAPTER_45_FILES})

target_include_directories(Chapter45 PRIVATE "${CMAKE_SOURCE_DIR}/include")
target_include_directories(Chapter45 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/glfw/include")
target_include_directories(Chapter45 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/glad/include")
target_include_directories(Chapter45 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/glm")
target_include_directories(Chapter45 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/stb")

target_link_libraries(Chapter45 glfw)
target_link_libraries(Chapter45 glm)

set_target_properties(Chapter45 PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/Chapter45")