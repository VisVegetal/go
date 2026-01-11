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
    message(STATUS "Configuring for macOS – forcing Apple Clang")

    set(CMAKE_C_COMPILER clang CACHE STRING "" FORCE)
    set(CMAKE_CXX_COMPILER clang++ CACHE STRING "" FORCE)
    set(CMAKE_OBJC_COMPILER clang CACHE STRING "" FORCE)
    set(CMAKE_OBJCXX_COMPILER clang++ CACHE STRING "" FORCE)
endif()

# =========================================
# Configurare SFML
# =========================================
# Dacă rulezi cu ASan pe non-Windows: e mai sigur să construiești SFML shared
# (evită probleme cu runtime / allocator)
if((CMAKE_CXX_FLAGS MATCHES "-fsanitize=address" OR CMAKE_C_FLAGS MATCHES "-fsanitize=address") AND NOT WIN32)
    message(STATUS "AddressSanitizer detected: building SFML shared and using dynamic stdlib")
    set(BUILD_SHARED_LIBS TRUE CACHE BOOL "" FORCE)
    set(SFML_USE_STATIC_STD_LIBS FALSE CACHE BOOL "" FORCE)
else()
    # SFML static + runtime consistent
    set(BUILD_SHARED_LIBS FALSE CACHE BOOL "" FORCE)
    set(SFML_USE_STATIC_STD_LIBS TRUE CACHE BOOL "" FORCE)
endif()

# Modulele SFML folosite de proiect
set(SFML_BUILD_AUDIO   FALSE CACHE BOOL "" FORCE)
set(SFML_BUILD_NETWORK FALSE CACHE BOOL "" FORCE)
set(SFML_BUILD_GRAPHICS TRUE CACHE BOOL "" FORCE)
set(SFML_BUILD_WINDOW   TRUE CACHE BOOL "" FORCE)

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

# Workaround-uri pentru Clang 18 + SFML
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-Wno-error=deprecated-declarations)
    add_compile_options(-Wno-deprecated-declarations)
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
    target_compile_options(${MAIN_EXECUTABLE_NAME} PRIVATE
            -Wno-deprecated-declarations
            -Wno-unused-private-field
    )
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
# Link SFML libraries (o singură dată)
# =========================================
target_link_libraries(${MAIN_EXECUTABLE_NAME} PRIVATE
        sfml-graphics
        sfml-window
        sfml-system
)

# Windows system libraries
if(WIN32)
    target_link_libraries(${MAIN_EXECUTABLE_NAME} PRIVATE opengl32 winmm gdi32)
endif()

# =========================================
# Include directories
# =========================================
target_include_directories(${MAIN_EXECUTABLE_NAME} PRIVATE
        ${sfml_SOURCE_DIR}/include
)

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
