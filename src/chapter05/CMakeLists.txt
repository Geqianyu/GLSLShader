file(GLOB_RECURSE CHAPTER_05_FILES
    ${CMAKE_SOURCE_DIR}/vendor/glad/src/gl.c
    ${CMAKE_SOURCE_DIR}/include/chapter05/*.h
    ${CMAKE_SOURCE_DIR}/src/chapter05/*.cpp)

add_executable(Chapter05 ${CHAPTER_05_FILES})

target_include_directories(Chapter05 PRIVATE "${CMAKE_SOURCE_DIR}/include")
target_include_directories(Chapter05 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/glfw/include")
target_include_directories(Chapter05 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/glad/include")
target_include_directories(Chapter05 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/glm")

target_link_libraries(Chapter05 glfw)
target_link_libraries(Chapter05 glm)

set_target_properties(Chapter05 PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/Chapter05")