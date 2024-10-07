cmake_minimum_required(VERSION 3.28)
project(HelloWorldSDL C)

set(FLECS_VERSION 4.0.2)

# Set the C standard
set(CMAKE_C_STANDARD 99)

# Find SDL2
find_package(SDL2 REQUIRED)

# Find Flecs
include(FetchContent)
FetchContent_Declare(
  flecs
  GIT_REPOSITORY https://github.com/SanderMertens/flecs.git
  GIT_TAG v${FLECS_VERSION} # Use the appropriate version tag
)
FetchContent_MakeAvailable(flecs)

# Add the executable
add_executable(${PROJECT_NAME} main.c)

# Link the libraries
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})
target_link_libraries(${PROJECT_NAME} flecs_static)