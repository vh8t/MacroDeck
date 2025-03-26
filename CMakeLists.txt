cmake_minimum_required(VERSION 3.16)
project(MacroPad
        VERSION 0.0.1
        DESCRIPTION "MacroPad Application"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -DNDEBUG")
set(CMAKE_C_FLAGS_RELEASE "-O3 -march=native -DNDEBUG")
set(CMAKE_BUILD_TYPE Release)

include_directories(${PROJECT_SOURCE_DIR}/include)

find_package(ALSA REQUIRED)

file(GLOB SOURCES "src/*.cpp")

add_executable(MacroPad ${SOURCES})

if (WIN32)

elseif(UNIX AND NOT APPLE)

endif()

set(VERSION_TAG "-dev")

set(PROJECT_FULL_VERSION "${PROJECT_VERSION}${VERSION_TAG}")
message(STATUS "${PROJECT_NAME} version: ${PROJECT_FULL_VERSION}")

target_link_libraries(MacroPad ${ALSA_LIBRARIES})

# install(TARGETS MacroPad DESTINATION bin)
