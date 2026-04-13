cmake_minimum_required(VERSION 3.24)
project(ReplantedMaxxer VERSION 0.5 LANGUAGES C CXX)

include(FetchContent)

set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 23)

set(CMAKE_C_COMPILER_TARGET x86_64-pc-windows-msvc)
set(CMAKE_CXX_COMPILER_TARGET x86_64-pc-windows-msvc)

add_link_options(/machine:x64)

# ------------------------
# Compiler-specific flags
# ------------------------
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  # MSVC: warning level 4, treat warnings as errors, disable unreferenced parameter warning
  add_compile_options(/W4 /WX /wd4100)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  # Clang/GCC: custom warnings
  add_compile_options(
        -Wno-error
        -Wno-delayed-template-parsing-in-cxx20
        -Wno-c++98-compat
        -Wno-c++98-compat-pedantic
        -Wno-newline-eof
        -Wno-padded
        -Wno-shadow-field-in-constructor
        -Wno-unsafe-buffer-usage
        -Wno-old-style-cast
        -Wno-exit-time-destructors
        -Wno-global-constructors
        -Wno-missing-prototypes
        -Wno-missing-variable-declarations
        -Wno-unique-object-duplication
        -Wzero-as-null-pointer-constant
        -Wunused-macros
        -Wmicrosoft-cast
        -Wreserved-macro-identifier
        -Wsign-conversion
        -Wextra-semi-stmt
        -Wnrvo
        -Wunused-parameter
        -Wswitch-default
    )
endif()

FetchContent_Declare(
  minhook
  GIT_REPOSITORY https://github.com/TsudaKageyu/minhook.git
  GIT_TAG 1e9ad1eb42db11bfcb65461f687c656612d1b555
)

FetchContent_Declare(
  kiero
  GIT_REPOSITORY https://github.com/Rebzzel/kiero.git
  GIT_TAG 177c3cf43a1b1be2651dba2a1acb3b334899ead7
)

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG 934c6a5f5ef2355d6df25395d555cb71f790c4e9
)

FetchContent_MakeAvailable(minhook kiero imgui)

# ------------------------
# Include directories
# ------------------------
include_directories(
    ${CMAKE_SOURCE_DIR}/Include
    ${kiero_SOURCE_DIR}
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

# ------------------------
# Project source files
# ------------------------
file(GLOB_RECURSE PROJECT_SOURCES_HH
    ${CMAKE_SOURCE_DIR}/Include/*.hpp
    ${CMAKE_SOURCE_DIR}/Include/*.h
)

file(GLOB_RECURSE PROJECT_SOURCES_CXX
    ${CMAKE_SOURCE_DIR}/Source/*.cpp
    ${CMAKE_SOURCE_DIR}/Source/*.h
)

# ------------------------
# ThirdParty source files
# ------------------------
file(GLOB_RECURSE THIRDPARTY_SOURCES_HH
    ${CMAKE_SOURCE_DIR}/ThirdParty/*.hpp
    ${CMAKE_SOURCE_DIR}/ThirdParty/*.h
)

file(GLOB_RECURSE THIRDPARTY_SOURCES_CXX
    ${CMAKE_SOURCE_DIR}/ThirdParty/*.cpp
    ${CMAKE_SOURCE_DIR}/ThirdParty/*.c
)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W4 -WX")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
endif()

add_library(${CMAKE_PROJECT_NAME} SHARED
    ${PROJECT_SOURCES_CXX}
    ${PROJECT_SOURCES_HH}
    ${THIRDPARTY_SOURCES_CXX}
    ${THIRDPARTY_SOURCES_HH}
)

# ------------------------
# Link libraries
# ------------------------
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE
    minhook
    d3d11.lib
    dxgi.lib
)

# Include this bit to export our proxy function as DirectInput8Create
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    "${CMAKE_SOURCE_DIR}/DllProxy.def"
)

# ------------------------
# Post-build copy
# ------------------------
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:${CMAKE_PROJECT_NAME}>"
        "$<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>"
)
