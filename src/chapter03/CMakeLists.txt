file(GLOB_RECURSE CHAPTER_03_FILES
    ${CMAKE_SOURCE_DIR}/vendor/glad/src/gl.c
    ${CMAKE_SOURCE_DIR}/include/chapter03/*.h
    ${CMAKE_SOURCE_DIR}/src/chapter03/*.cpp)

add_executable(Chapter03 ${CHAPTER_03_FILES})

target_include_directories(Chapter03 PRIVATE "${CMAKE_SOURCE_DIR}/include")
target_include_directories(Chapter03 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/glfw/include")
target_include_directories(Chapter03 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/glad/include")
target_include_directories(Chapter03 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/glm")

target_link_libraries(Chapter03 glfw)
target_link_libraries(Chapter03 glm)

set_target_properties(Chapter03 PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/Chapter03")