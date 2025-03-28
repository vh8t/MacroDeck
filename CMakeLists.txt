cmake_minimum_required(VERSION 3.16)
project(MacroPad
        VERSION 0.0.4
        DESCRIPTION "MacroPad Application"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -DNDEBUG")
set(CMAKE_C_FLAGS_RELEASE "-O3 -march=native -DNDEBUG")
set(CMAKE_BUILD_TYPE Release)

include_directories(${PROJECT_SOURCE_DIR}/include)

find_package(PkgConfig REQUIRED)

pkg_check_modules(ALSA REQUIRED alsa)

file(GLOB SOURCES "src/*.cpp")

add_executable(MacroPad ${SOURCES})

if (WIN32)

elseif(UNIX AND NOT APPLE)

endif()

set(VERSION_TAG "-dev")
set(PROJECT_FULL_VERSION "${PROJECT_VERSION}${VERSION_TAG}")
message(STATUS "${PROJECT_NAME} version: ${PROJECT_FULL_VERSION}")

target_include_directories(MacroPad PRIVATE ${ALSA_INCLUDE_DIRS})
target_link_libraries(MacroPad PRIVATE ${ALSA_LIBRARIES})
target_compile_options(MacroPad PRIVATE ${ALSA_CFLAGS_OTHER})

# install(TARGETS MacroPad DESTINATION bin)
