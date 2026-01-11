cmake_minimum_required(VERSION 3.26)

# =========================================
# Proiect
# =========================================
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

# =========================================
# Detectare platforma
# =========================================
if(APPLE)
    # prefer system clang; fall back to finding clang on PATH
    if(NOT DEFINED CMAKE_OBJC_COMPILER OR CMAKE_OBJC_COMPILER STREQUAL "")
        find_program(_APPLE_CLANG clang HINTS /usr/bin /usr/local/bin /opt/homebrew/bin)
        if(NOT _APPLE_CLANG)
            find_program(_APPLE_CLANG clang)
        endif()
        if(_APPLE_CLANG)
            message(STATUS "Forcing Objective-C compiler to Apple clang: ${_APPLE_CLANG}")
            set(CMAKE_OBJC_COMPILER "${_APPLE_CLANG}" CACHE FILEPATH "Objective-C compiler" FORCE)
            # clang driver also handles ObjC++
            set(CMAKE_OBJCXX_COMPILER "${_APPLE_CLANG}" CACHE FILEPATH "Objective-C++ compiler" FORCE)
        else()
            message(WARNING "Apple clang not found; Objective-C files may fail to compile with GCC.")
        endif()
    endif()
endif()

# =========================================
# Configurare SFML
# =========================================
# Dezactivăm AddressSanitizer pe Windows
if((CMAKE_CXX_FLAGS MATCHES "-fsanitize=address" OR CMAKE_C_FLAGS MATCHES "-fsanitize=address") AND NOT WIN32)
    message(STATUS "AddressSanitizer detected: building SFML shared and using dynamic stdlib")
    set(BUILD_SHARED_LIBS TRUE CACHE BOOL "" FORCE)
    set(SFML_USE_STATIC_STD_LIBS FALSE CACHE BOOL "" FORCE)
else()
    # Folosim SFML static + runtime consistent
    set(BUILD_SHARED_LIBS FALSE CACHE BOOL "" FORCE)
    set(SFML_USE_STATIC_STD_LIBS TRUE CACHE BOOL "" FORCE)
endif()

# SFML modules
set(SFML_BUILD_AUDIO FALSE CACHE BOOL "" FORCE)
set(SFML_BUILD_NETWORK FALSE CACHE BOOL "" FORCE)
###asta
set(SFML_BUILD_GRAPHICS TRUE CACHE BOOL "" FORCE)
set(SFML_BUILD_WINDOW TRUE CACHE BOOL "" FORCE)

# Graphics & Window modules
if(WIN32 OR APPLE)
    set(SFML_BUILD_GRAPHICS TRUE CACHE BOOL "" FORCE)
    set(SFML_BUILD_WINDOW TRUE CACHE BOOL "" FORCE)
else()
    set(SFML_BUILD_GRAPHICS OFF CACHE BOOL "" FORCE)
    set(SFML_BUILD_WINDOW OFF CACHE BOOL "" FORCE)
endif()

# =========================================
# Fetch SFML
# =========================================
include(FetchContent)
FetchContent_Declare(
        sfml
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.6.1
)
FetchContent_MakeAvailable(sfml)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # Ignoră eroarea de deprecation pentru char_traits (necesar pentru SFML + Clang 18)
    add_compile_options(-Wno-error=deprecated-declarations)
    add_compile_options(-Wno-deprecated-declarations)

    # Ignoră eroarea de câmpuri nefolosite (pentru negru/alb din Joc.hpp)
    add_compile_options(-Wno-error=unused-private-field)
    add_compile_options(-Wno-unused-private-field)
endif()

# =========================================
# Executabil
# =========================================
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

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # Dezactivează eroarea pentru deprecerea char_traits<unsigned int>
    target_compile_options(${MAIN_EXECUTABLE_NAME} PRIVATE -Wno-deprecated-declarations)

    # Dacă ai erori și pentru câmpuri nefolosite (unused-private-field)
    target_compile_options(${MAIN_EXECUTABLE_NAME} PRIVATE -Wno-unused-private-field)
endif()
# Enable sanitizers (doar pe non-Windows)
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

# =========================================
# Link SFML libraries
# =========================================
target_link_libraries(${MAIN_EXECUTABLE_NAME} PRIVATE sfml-system)

if(SFML_BUILD_AUDIO)
    target_link_libraries(${MAIN_EXECUTABLE_NAME} PRIVATE sfml-audio)
endif()

if(SFML_BUILD_GRAPHICS)
    target_link_libraries(${MAIN_EXECUTABLE_NAME} PRIVATE sfml-graphics sfml-window)
endif()

# Windows system libraries
if(WIN32)
    target_link_libraries(${MAIN_EXECUTABLE_NAME} PRIVATE opengl32 winmm gdi32)
endif()

# =========================================
# Include directories
# =========================================
target_include_directories(${MAIN_EXECUTABLE_NAME} PRIVATE ${sfml_SOURCE_DIR}/include)

# =========================================
# Install & copy resources
# =========================================
install(TARGETS ${MAIN_EXECUTABLE_NAME} DESTINATION ${DESTINATION_DIR})

if(APPLE)
    install(FILES launcher.command DESTINATION ${DESTINATION_DIR})
endif()

copy_files(
        FILES tastatura.txt arial.ttf
        COPY_TO_DESTINATION
        TARGET_NAME ${MAIN_EXECUTABLE_NAME}
)