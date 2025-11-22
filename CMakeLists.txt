cmake_minimum_required(VERSION 3.10)
project(RevvedUp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(SFML_BUILD_NETWORK "Build network" OFF)
set(SFML_DIR "external/SFML")
add_subdirectory(external/SFML)

if(DEFINED DEV_PHASE)
    set(IMGUI_DIR ${PROJECT_SOURCE_DIR}/external/imgui)
    option(IMGUI_SFML_FIND_SFML "Use find_package to find SFML" OFF)
    option(IMGUI_SFML_IMGUI_DEMO "Build imgui_demo.cpp" ON)
    option(IMGUI_SFML_DISABLE_OBSOLETE_FUNCTIONS "Disable obsolete ImGui functions" ON)
    add_subdirectory(external/imgui-sfml)
endif()

file(COPY ${PROJECT_SOURCE_DIR}/assets DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

file(GLOB_RECURSE SOURCES "src/*.cpp") 
add_executable(${PROJECT_NAME} ${SOURCES}) 

target_include_directories(${PROJECT_NAME} PRIVATE
    external/SFML/include
    include
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    sfml-system
    sfml-graphics
    sfml-window
    sfml-audio
)


if(DEFINED DEV_PHASE)
    target_compile_definitions(${PROJECT_NAME} PRIVATE DEV_PHASE=${DEV_PHASE})

    target_include_directories(${PROJECT_NAME} PRIVATE
        ${IMGUI_DIR}
        external/imgui-sfml
    )

    target_link_libraries(${PROJECT_NAME} PRIVATE
        ImGui-SFML
    )
endif()
