cmake_minimum_required(VERSION 3.28)
project(flecs-sdl-test C)

set(FLECS_VERSION 4.0.2)
set(NUKLEAR_VERSION 4.12.0)

# Set the C standard
set(CMAKE_C_STANDARD 99)

# Find SDL2
find_package(SDL2 REQUIRED)

include(FetchContent)
# Find Flecs
FetchContent_Declare(
  flecs
  GIT_REPOSITORY https://github.com/SanderMertens/flecs.git
  GIT_TAG v${FLECS_VERSION} # Use the appropriate version tag
)
# Find Nuklear
# FetchContent_Declare(
#   nuklear
#   GIT_REPOSITORY git@github.com:Immediate-Mode-UI/Nuklear.git
#   GIT_TAG ${NUKLEAR_VERSION} # Use the appropriate version tag
# )
FetchContent_MakeAvailable(flecs)
# FetchContent_MakeAvailable(nuklear)

include_directories(vendor/nuklear)

# Add the executable
add_executable(${PROJECT_NAME} main.c)

# Link the libraries
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})
target_link_libraries(${PROJECT_NAME} flecs_static)
