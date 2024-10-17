project(simple_components C)

add_library(${PROJECT_NAME} STATIC main.c)

include_directories(${flecs_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} flecs_static)
