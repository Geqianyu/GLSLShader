file(GLOB_RECURSE CHAPTER_04_FILES
    ${CMAKE_SOURCE_DIR}/vendor/glad/src/gl.c
    ${CMAKE_SOURCE_DIR}/include/chapter04/*.h
    ${CMAKE_SOURCE_DIR}/src/chapter04/*.cpp)

add_executable(Chapter04 ${CHAPTER_04_FILES})

target_include_directories(Chapter04 PRIVATE "${CMAKE_SOURCE_DIR}/include")
target_include_directories(Chapter04 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/glfw/include")
target_include_directories(Chapter04 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/glad/include")
target_include_directories(Chapter04 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/glm")

target_link_libraries(Chapter04 glfw)
target_link_libraries(Chapter04 glm)

set_target_properties(Chapter04 PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/Chapter04")