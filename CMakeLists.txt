cmake_minimum_required(VERSION 3.26)

# NOTE: update executable name in .github/workflows/cmake.yml:25 when changing executable name in this file
set(MAIN_PROJECT_NAME "oop")
set(MAIN_EXECUTABLE_NAME "${MAIN_PROJECT_NAME}")

project(${MAIN_PROJECT_NAME})

if(APPLE AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-include dispatch/dispatch.h)
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(cmake/Options.cmake)
include(cmake/CompilerFlags.cmake)
include(cmake/CopyHelper.cmake)


###############################################################################
# SFML configuration
if(CMAKE_CXX_FLAGS MATCHES "-fsanitize=address" OR CMAKE_C_FLAGS MATCHES "-fsanitize=address")
    message(STATUS "AddressSanitizer detected: building SFML shared and using dynamic stdlib")
    set(BUILD_SHARED_LIBS TRUE CACHE BOOL "" FORCE)
    set(SFML_USE_STATIC_STD_LIBS FALSE CACHE BOOL "" FORCE)
else()
    set(BUILD_SHARED_LIBS FALSE CACHE BOOL "" FORCE)
    set(SFML_USE_STATIC_STD_LIBS TRUE CACHE BOOL "" FORCE)
endif()

set(SFML_BUILD_AUDIO FALSE CACHE BOOL "" FORCE)
set(SFML_BUILD_NETWORK FALSE CACHE BOOL "" FORCE)

# Disable OpenGL-dependent modules on Linux CI
if(UNIX AND NOT APPLE)
    set(SFML_BUILD_GRAPHICS OFF CACHE BOOL "" FORCE)
    set(SFML_BUILD_WINDOW OFF CACHE BOOL "" FORCE)
else()
    set(SFML_BUILD_GRAPHICS TRUE CACHE BOOL "" FORCE)
    set(SFML_BUILD_WINDOW TRUE CACHE BOOL "" FORCE)
endif()

###############################################################################
# Fetch SFML

include(FetchContent)
FetchContent_Declare(
        sfml
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.6.1
)

FetchContent_MakeAvailable(sfml)

###############################################################################
# Executable

add_executable(${MAIN_EXECUTABLE_NAME}
        main.cpp
        Jucator.hpp
        Jucator.cpp
        Joc.hpp
        Joc.cpp
        GoExceptions.hpp
        Culoare.hpp
        Pozitie.hpp
        Tabla.cpp
        Tabla.hpp
        Mutare.hpp
        Reguli.cpp
        Reguli.hpp
)


# Enable sanitizers on CI
set_compiler_flags(
        RUN_SANITIZERS TRUE
        TARGET_NAMES ${MAIN_EXECUTABLE_NAME}
)

###############################################################################
# Link libraries

target_link_libraries(${MAIN_EXECUTABLE_NAME} PRIVATE
        sfml-system
)

if(SFML_BUILD_AUDIO)
    target_link_libraries(${MAIN_EXECUTABLE_NAME} PRIVATE
            sfml-audio
    )
endif()

# Graphics only where available
if(NOT (UNIX AND NOT APPLE))
    target_link_libraries(${MAIN_EXECUTABLE_NAME} PRIVATE
            sfml-graphics
            sfml-window
    )
endif()

# Windows system libraries
if(WIN32)
    target_link_libraries(${MAIN_EXECUTABLE_NAME} PRIVATE
            opengl32
            winmm
            gdi32
    )
endif()

###############################################################################
# Includes

target_include_directories(
        ${MAIN_EXECUTABLE_NAME}
        PRIVATE
        ${sfml_SOURCE_DIR}/include
)

###############################################################################
# Install & copy resources

install(TARGETS ${MAIN_EXECUTABLE_NAME} DESTINATION ${DESTINATION_DIR})

if(APPLE)
    install(FILES launcher.command DESTINATION ${DESTINATION_DIR})
endif()

copy_files(
        FILES tastatura.txt
        COPY_TO_DESTINATION
        TARGET_NAME ${MAIN_EXECUTABLE_NAME}
)
