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

# =========================================
# Detectare platforma
# =========================================
if(APPLE)
    set(CMAKE_CXX_COMPILER "clang++" CACHE STRING "C++ compiler" FORCE)
endif()

# =========================================
# Configurare SFML
# =========================================
# Dezactivăm AddressSanitizer pe Windows
if((CMAKE_CXX_FLAGS MATCHES "-fsanitize=address" OR CMAKE_C_FLAGS MATCHES "-fsanitize=address") AND NOT WIN32)
    message(STATUS "AddressSanitizer detected: building SFML shared")
    set(BUILD_SHARED_LIBS TRUE CACHE BOOL "" FORCE)
    set(SFML_USE_STATIC_STD_LIBS FALSE CACHE BOOL "" FORCE)
else()
    set(BUILD_SHARED_LIBS FALSE CACHE BOOL "" FORCE)
    set(SFML_USE_STATIC_STD_LIBS TRUE CACHE BOOL "" FORCE)
endif()

# ACTIVĂM modulele necesare pentru toate platformele
set(SFML_BUILD_GRAPHICS TRUE CACHE BOOL "" FORCE)
set(SFML_BUILD_WINDOW TRUE CACHE BOOL "" FORCE)
set(SFML_BUILD_AUDIO FALSE CACHE BOOL "" FORCE)
set(SFML_BUILD_NETWORK FALSE CACHE BOOL "" FORCE)

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

# =========================================
# Executabil
# =========================================
add_executable(${MAIN_EXECUTABLE_NAME}
        main.cpp
        Jucator.hpp Jucator.cpp
        Joc.hpp Joc.cpp
        GoExceptions.hpp
        Culoare.hpp
        Pozitie.hpp
        Tabla.cpp Tabla.hpp
        Mutare.hpp
        Reguli.cpp Reguli.hpp
)

# =========================================
# Link SFML libraries (IMPORTANT)
# =========================================
# Pe Linux/Ubuntu, avem nevoie de sfml-graphics care depinde de window și system
target_link_libraries(${MAIN_EXECUTABLE_NAME} PRIVATE sfml-graphics sfml-window sfml-system)

if(WIN32)
    target_link_libraries(${MAIN_EXECUTABLE_NAME} PRIVATE opengl32 winmm gdi32)
endif()

# =========================================
# Include directories
# =========================================
target_include_directories(${MAIN_EXECUTABLE_NAME} PRIVATE ${sfml_SOURCE_DIR}/include)

# =========================================
# Sanitizers & Resources
# =========================================
if(NOT WIN32)
    set_compiler_flags(RUN_SANITIZERS TRUE TARGET_NAMES ${MAIN_EXECUTABLE_NAME})
endif()

copy_files(
        FILES tastatura.txt arial.ttf
        COPY_TO_DESTINATION
        TARGET_NAME ${MAIN_EXECUTABLE_NAME}
)