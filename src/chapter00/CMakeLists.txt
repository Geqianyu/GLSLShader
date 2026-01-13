file(GLOB_RECURSE CHAPTER_00_FILES
    ${CMAKE_SOURCE_DIR}/vendor/glad/src/gl.c
    ${CMAKE_SOURCE_DIR}/include/chapter00/*.h
    ${CMAKE_SOURCE_DIR}/src/chapter00/*.cpp)

add_executable(Chapter00 ${CHAPTER_00_FILES})

target_include_directories(Chapter00 PRIVATE "${CMAKE_SOURCE_DIR}/include")
target_include_directories(Chapter00 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/glfw/include")
target_include_directories(Chapter00 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/glad/include")

target_link_libraries(Chapter00 glfw)

set_target_properties(Chapter00 PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/Chapter00")