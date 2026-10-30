# ---------------------------------------------------------------------------------------------------------------------
# The project
# ---------------------------------------------------------------------------------------------------------------------

cmake_minimum_required (VERSION 3.28)

set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<CONFIG:Debug,RelWithDebInfo>,ProgramDatabase,Embedded>")

project("SIFL" VERSION 1.0.2 LANGUAGES CXX)

option(SIFLTopLevel "is SIFL used as a library or as a standalone project?" ${PROJECT_IS_TOP_LEVEL})

if (SIFLTopLevel)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

    # MSVC flags to support modules later
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)
endif()

# ---------------------------------------------------------------------------------------------------------------------
# The project
# ---------------------------------------------------------------------------------------------------------------------

# ---------------------------------------------------------------------------------------------------------------------
# Toolchain infos
# ---------------------------------------------------------------------------------------------------------------------

if (SIFLTopLevel)
    message(STATUS "The compiler found is '${CMAKE_CXX_COMPILER_ID}'")
    message(STATUS "The compiler version is '${CMAKE_CXX_COMPILER_VERSION}'")
    message(STATUS "The standard library found is '${CMAKE_CXX_STANDARD_LIBRARY}'")
    message(STATUS "The CMake version found is '${CMAKE_VERSION}'")
    message(STATUS "The generator found is '${CMAKE_GENERATOR}'")
endif()

# Toolchain checks
if (NOT CMAKE_GENERATOR MATCHES "Ninja|Visual Studio")
    message(WARNING "Only Ninja and Visual Studio have been tested with SIFL and modules/`import std;`")
endif ()

if (NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU|MSVC")
    message(WARNING "Only Clang, GCC and MSVC have been tested with SIFL")
endif ()

#todo: modules
#if (CMAKE_GENERATOR MATCHES "Ninja")
#    if (NOT "23" IN_LIST CMAKE_CXX_COMPILER_IMPORT_STD)
#        message(WARNING "Ninja does not report `import std;` support for C++23 ")
#    endif ()
#endif ()

# ---------------------------------------------------------------------------------------------------------------------
# Toolchain infos
# ---------------------------------------------------------------------------------------------------------------------

# ---------------------------------------------------------------------------------------------------------------------
# dependency management
# ---------------------------------------------------------------------------------------------------------------------

include(FetchContent)

if (SIFLTopLevel)
    set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE)
endif ()

FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG        3.1.0
        GIT_SHALLOW    ON
        EXCLUDE_FROM_ALL
        SYSTEM
)
FetchContent_MakeAvailable(SFML)

# ---------------------------------------------------------------------------------------------------------------------
# dependency management
# ---------------------------------------------------------------------------------------------------------------------

# ---------------------------------------------------------------------------------------------------------------------
# The library target
# ---------------------------------------------------------------------------------------------------------------------

#todo: modules
# Additional steps to support modules/`import std` with Ninja.
#if (CMAKE_GENERATOR MATCHES "Ninja")
#    set(CMAKE_CXX_SCAN_FOR_MODULES ON)
#    set(CMAKE_CXX_MODULE_STD ON)
#endif()

set(SOURCE_HEADERS #todo MODULE_INTERFACES
        src/SIFL/GraphicalResources.hpp
        src/SIFL/BasicInterface.hpp
        src/SIFL/MutableInterface.hpp
        src/SIFL/InteractiveInterface.hpp
        src/SIFL/CompoundElements.hpp
        src/SIFL/GUI.hpp
)

set(SOURCE_IMPLEMENTATIONS #todo MODULE_IMPLEMENTATIONS
        src/SIFL/GraphicalResources.cpp
        src/SIFL/BasicInterface.cpp
        src/SIFL/MutableInterface.cpp
        src/SIFL/InteractiveInterface.cpp
        src/SIFL/CompoundElements.cpp
        src/SIFL/GUI.cpp
)

add_library(${PROJECT_NAME})
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20) #todo cxx_std_23

target_include_directories(${PROJECT_NAME}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(${PROJECT_NAME}
        PUBLIC
        SFML::Graphics
        SFML::Window
        SFML::System
)

target_sources(${PROJECT_NAME}
        PUBLIC
            ${SOURCE_HEADERS}
        PRIVATE
            ${SOURCE_IMPLEMENTATIONS}
)

# This is used to load the font.
target_compile_definitions(${PROJECT_NAME}
        PUBLIC
        SIFL_ASSET_DIR="${CMAKE_CURRENT_SOURCE_DIR}/assets/"
)

# ---------------------------------------------------------------------------------------------------------------------
# The library target
# ---------------------------------------------------------------------------------------------------------------------

# ---------------------------------------------------------------------------------------------------------------------
# The test executable
# ---------------------------------------------------------------------------------------------------------------------

if (SIFLTopLevel)
    set(source_files_executable
            ${SOURCE_HEADERS}
            ${SOURCE_IMPLEMENTATIONS}
            "tests/executable/example.cpp"
    )

    add_executable(testExecutable ${source_files_executable})
    target_compile_features(testExecutable PRIVATE cxx_std_20) #todo cxx_std_23
    target_link_libraries(testExecutable PRIVATE
            SFML::System
            SFML::Window
            SFML::Graphics
    )
endif ()

# ---------------------------------------------------------------------------------------------------------------------
# The test executable
# ---------------------------------------------------------------------------------------------------------------------