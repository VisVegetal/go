cmake_minimum_required(VERSION 3.26)


set(MAIN_PROJECT_NAME "oop")
set(MAIN_EXECUTABLE_NAME "${MAIN_PROJECT_NAME}")

project(${MAIN_PROJECT_NAME} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(cmake/Options.cmake)
include(cmake/CompilerFlags.cmake)
include(cmake/CopyHelper.cmake)

if(APPLE)
    message(STATUS "Configuring for macOS – forcing Apple Clang")

    set(CMAKE_C_COMPILER clang CACHE STRING "" FORCE)
    set(CMAKE_CXX_COMPILER clang++ CACHE STRING "" FORCE)
    set(CMAKE_OBJC_COMPILER clang CACHE STRING "" FORCE)
    set(CMAKE_OBJCXX_COMPILER clang++ CACHE STRING "" FORCE)
endif()

option(GO_HEADLESS "Build without SFML window/graphics" OFF)

if(DEFINED ENV{GITHUB_ACTIONS} AND UNIX AND NOT APPLE)
    set(GO_HEADLESS ON CACHE BOOL "" FORCE)
endif()

if(GO_HEADLESS)
    message(STATUS "GO_HEADLESS=ON (SFML window/graphics disabled)")
endif()


if((CMAKE_CXX_FLAGS MATCHES "-fsanitize=address" OR CMAKE_C_FLAGS MATCHES "-fsanitize=address") AND NOT WIN32)
    message(STATUS "AddressSanitizer detected: building SFML shared and using dynamic stdlib")
    set(BUILD_SHARED_LIBS TRUE CACHE BOOL "" FORCE)
    set(SFML_USE_STATIC_STD_LIBS FALSE CACHE BOOL "" FORCE)
else()
    set(BUILD_SHARED_LIBS FALSE CACHE BOOL "" FORCE)
    set(SFML_USE_STATIC_STD_LIBS TRUE CACHE BOOL "" FORCE)
endif()

# Module SFML
set(SFML_BUILD_AUDIO   FALSE CACHE BOOL "" FORCE)
set(SFML_BUILD_NETWORK FALSE CACHE BOOL "" FORCE)

if(GO_HEADLESS)
    set(SFML_BUILD_GRAPHICS FALSE CACHE BOOL "" FORCE)
    set(SFML_BUILD_WINDOW   FALSE CACHE BOOL "" FORCE)
else()
    set(SFML_BUILD_GRAPHICS TRUE CACHE BOOL "" FORCE)
    set(SFML_BUILD_WINDOW   TRUE CACHE BOOL "" FORCE)
endif()

include(FetchContent)
FetchContent_Declare(
        sfml
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.6.1
)
FetchContent_MakeAvailable(sfml)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-Wno-error=deprecated-declarations)
    add_compile_options(-Wno-deprecated-declarations)
    add_compile_options(-Wno-error=unused-private-field)
    add_compile_options(-Wno-unused-private-field)
endif()


add_executable(${MAIN_EXECUTABLE_NAME}
        src/main.cpp
        include/Jucator.hpp
        src/Jucator.cpp
        include/Joc.hpp
        src/Joc.cpp
        include/GoExceptions.hpp
        include/Culoare.hpp
        include/Pozitie.hpp
        src/Tabla.cpp
        include/Tabla.hpp
        include/Mutare.hpp
        src/Reguli.cpp
        include/Reguli.hpp
        src/GoLogic.cpp
        include/GoLogic.hpp
        include/BotStrategii.hpp
        src/BotStrategii.cpp
)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(${MAIN_EXECUTABLE_NAME} PRIVATE
            -Wno-deprecated-declarations
            -Wno-unused-private-field
    )
endif()

if(NOT WIN32)
    set_compiler_flags(
            RUN_SANITIZERS TRUE
            TARGET_NAMES ${MAIN_EXECUTABLE_NAME}
    )
endif()

if(DEFINED ENV{GITHUB_ACTIONS})
    add_definitions(-DCH_CI_RUN)
    message(STATUS "Configuring for GitHub Actions - UI tests will be bypassed")
endif()

if(GO_HEADLESS)
    target_link_libraries(${MAIN_EXECUTABLE_NAME} PRIVATE sfml-system)
    target_compile_definitions(${MAIN_EXECUTABLE_NAME} PRIVATE GO_HEADLESS=1)
else()
    target_link_libraries(${MAIN_EXECUTABLE_NAME} PRIVATE sfml-graphics sfml-window sfml-system)
endif()

# Windows system libraries
if(WIN32)
    target_link_libraries(${MAIN_EXECUTABLE_NAME} PRIVATE opengl32 winmm gdi32)
endif()

target_include_directories(${MAIN_EXECUTABLE_NAME} PRIVATE ${sfml_SOURCE_DIR}/include)

install(TARGETS ${MAIN_EXECUTABLE_NAME} DESTINATION ${DESTINATION_DIR})

if(APPLE)
    install(FILES launcher.command DESTINATION ${DESTINATION_DIR})
endif()

copy_files(
        FILES tastatura.txt arial.ttf
        COPY_TO_DESTINATION
        TARGET_NAME ${MAIN_EXECUTABLE_NAME}
)
