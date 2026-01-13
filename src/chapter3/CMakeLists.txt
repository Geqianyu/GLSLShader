file(GLOB_RECURSE CHAPTER_3_FILES
    ${CMAKE_SOURCE_DIR}/vendor/glad/src/gl.c
    ${CMAKE_SOURCE_DIR}/include/chapter3/*.h
    ${CMAKE_SOURCE_DIR}/src/chapter3/*.cpp)

add_executable(Chapter3 ${CHAPTER_3_FILES})

target_include_directories(Chapter3 PRIVATE "${CMAKE_SOURCE_DIR}/include")
target_include_directories(Chapter3 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/glfw/include")
target_include_directories(Chapter3 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/glad/include")
target_include_directories(Chapter3 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/glm")

target_link_libraries(Chapter3 glfw)
target_link_libraries(Chapter3 glm)

set_target_properties(Chapter3 PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/Chapter3")