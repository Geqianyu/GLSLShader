file(GLOB_RECURSE CHAPTER_01_FILES
    ${CMAKE_SOURCE_DIR}/vendor/glad/src/gl.c
    ${CMAKE_SOURCE_DIR}/include/chapter01/*.h
    ${CMAKE_SOURCE_DIR}/src/chapter01/*.cpp)

add_executable(Chapter01 ${CHAPTER_01_FILES})

target_include_directories(Chapter01 PRIVATE "${CMAKE_SOURCE_DIR}/include")
target_include_directories(Chapter01 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/glfw/include")
target_include_directories(Chapter01 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/glad/include")

target_link_libraries(Chapter01 glfw)

set_target_properties(Chapter01 PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/Chapter01")