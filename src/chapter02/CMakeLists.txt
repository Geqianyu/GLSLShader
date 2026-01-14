file(GLOB_RECURSE CHAPTER_02_FILES
    ${CMAKE_SOURCE_DIR}/vendor/glad/src/gl.c
    ${CMAKE_SOURCE_DIR}/src/chapter02/*.cpp)

add_executable(Chapter02 ${CHAPTER_02_FILES})

target_include_directories(Chapter02 PRIVATE "${CMAKE_SOURCE_DIR}/include")
target_include_directories(Chapter02 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/glfw/include")
target_include_directories(Chapter02 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/glad/include")
target_include_directories(Chapter02 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/glm")

target_link_libraries(Chapter02 glfw)
target_link_libraries(Chapter02 glm)

set_target_properties(Chapter02 PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/Chapter02")