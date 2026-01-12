file(GLOB_RECURSE CHAPTER_2_FILES
    ${CMAKE_SOURCE_DIR}/vendor/glad/src/gl.c
    ${CMAKE_SOURCE_DIR}/include/chapter2/*.h
    ${CMAKE_SOURCE_DIR}/src/chapter2/*.cpp)

add_executable(Chapter2 ${CHAPTER_2_FILES})

target_include_directories(Chapter2 PRIVATE "${CMAKE_SOURCE_DIR}/include")
target_include_directories(Chapter2 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/glfw/include")
target_include_directories(Chapter2 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/glad/include")
target_include_directories(Chapter2 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/glm")

target_link_libraries(Chapter2 glfw)
target_link_libraries(Chapter2 glm)

set_target_properties(Chapter2 PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/Chapter2")