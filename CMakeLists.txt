cmake_minimum_required (VERSION 3.20)
project("SIFL3" VERSION 1.0.1 LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(SFML_BUILD_NETWORK OFF)

include(FetchContent)
FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG        3.1.0
        GIT_SHALLOW    ON
        EXCLUDE_FROM_ALL
        SYSTEM
)
FetchContent_MakeAvailable(SFML)

file(GLOB_RECURSE source_files
        "src/*.cpp"
        "src/*.hpp"
)
add_executable(${PROJECT_NAME} ${source_files})

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

target_link_libraries(${PROJECT_NAME} PRIVATE
        SFML::System
        SFML::Window
        SFML::Graphics
        SFML::Audio
)