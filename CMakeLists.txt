cmake_minimum_required(VERSION 3.28)
project(flecs-sdl-test C)

set(FLECS_VERSION 4.0.2)
set(NUKLEAR_VERSION 4.12.0)

# Set the C standard
set(CMAKE_C_STANDARD 99)

# set(CMAKE_C_CLANG_TIDY clang-tidy)

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
FetchContent_Declare(
  nuklear
  GIT_REPOSITORY git@github.com:Immediate-Mode-UI/Nuklear.git
  GIT_TAG ${NUKLEAR_VERSION} # Use the appropriate version tag
)
FetchContent_MakeAvailable(flecs)
FetchContent_MakeAvailable(nuklear)

include_directories(${nuklear_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/include)


add_subdirectory(modules/simple_components)
include_directories(${simple_components_SOURCE_DIR})

add_subdirectory(modules/simple_systems)
include_directories(${simple_systems_SOURCE_DIR})

# Add the executable
FILE(GLOB C_SOURCES src/*.c)
add_executable(${PROJECT_NAME} ${C_SOURCES})


# Link the libraries
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})
target_link_libraries(${PROJECT_NAME} flecs_static)
target_link_libraries(${PROJECT_NAME} simple_systems)
target_link_libraries(${PROJECT_NAME} simple_components)

get_cmake_property(_variableNames VARIABLES)
list (SORT _variableNames)
foreach (_variableName ${_variableNames})
    message(STATUS "${_variableName}=${${_variableName}}")
endforeach()