file(GLOB_RECURSE CHAPTER_0_FILES
    ${CMAKE_SOURCE_DIR}/vendor/glad/src/gl.c
    ${CMAKE_SOURCE_DIR}/include/common/*.h
    ${CMAKE_SOURCE_DIR}/src/common/*.cpp
    ${CMAKE_SOURCE_DIR}/include/chapter0/*.h
    ${CMAKE_SOURCE_DIR}/src/chapter0/*.cpp)

add_executable(Chapter0 ${CHAPTER_0_FILES})

target_include_directories(Chapter0 PRIVATE "${CMAKE_SOURCE_DIR}/include")
target_include_directories(Chapter0 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/glfw/include")
target_include_directories(Chapter0 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/glad/include")
target_include_directories(Chapter0 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/glm")

target_link_libraries(Chapter0 glfw)
target_link_libraries(Chapter0 glm)

set_target_properties(Chapter0 PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/Chapter0")