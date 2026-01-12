file(GLOB_RECURSE CHAPTER_1_FILES
    ${CMAKE_SOURCE_DIR}/vendor/glad/src/gl.c
    ${CMAKE_SOURCE_DIR}/include/chapter1/*.h
    ${CMAKE_SOURCE_DIR}/src/chapter1/*.cpp)

add_executable(Chapter1 ${CHAPTER_1_FILES})

target_include_directories(Chapter1 PRIVATE "${CMAKE_SOURCE_DIR}/include")
target_include_directories(Chapter1 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/glfw/include")
target_include_directories(Chapter1 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/glad/include")

target_link_libraries(Chapter1 glfw)

set_target_properties(Chapter1 PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/Chapter1")