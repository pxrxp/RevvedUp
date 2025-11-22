cmake_minimum_required(VERSION 3.22)
project(RevvedUp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(SFML_STATIC_LIBRARIES TRUE)

find_package(SFML 2.6 COMPONENTS graphics window system audio REQUIRED)
find_package(ImGui-SFML REQUIRED)

file(COPY ${PROJECT_SOURCE_DIR}/assets DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
file(GLOB_RECURSE SOURCES "src/*.cpp") 

add_executable(${PROJECT_NAME} ${SOURCES}) 

target_include_directories(${PROJECT_NAME} PRIVATE include)
target_link_libraries(${PROJECT_NAME} PRIVATE
    sfml-graphics-s
    sfml-window-s
    sfml-audio-s
    sfml-system-s

    freetype
    openal
    FLAC
    vorbisenc
    vorbisfile
    vorbis
    ogg
    udev

    X11
    Xrandr
    Xcursor
    Xrender
    Xfixes
    Xext

    pthread
    dl
)

if(DEFINED DEV_PHASE)
    target_link_libraries(${PROJECT_NAME} PRIVATE ImGui-SFML::ImGui-SFML)
endif()
