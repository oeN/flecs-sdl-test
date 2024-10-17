project(simple_systems C)

add_library(${PROJECT_NAME} main.c)

include_directories(${flecs_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} flecs_static)