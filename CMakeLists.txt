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
  add_compile_options(/wd4100)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
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

# ------------------------
# minhook
# ------------------------
FetchContent_Declare(
  minhook
  GIT_REPOSITORY https://github.com/TsudaKageyu/minhook.git
  GIT_TAG 1e9ad1eb42db11bfcb65461f687c656612d1b555
)
FetchContent_MakeAvailable(minhook)

# ------------------------
# kiero
# ------------------------
FetchContent_Declare(
  kiero
  GIT_REPOSITORY https://github.com/Rebzzel/kiero.git
  GIT_TAG 177c3cf43a1b1be2651dba2a1acb3b334899ead7
)

FetchContent_MakeAvailable(kiero)

add_library(
  kiero STATIC ${kiero_SOURCE_DIR}/kiero.cpp
)

target_include_directories(
  kiero PUBLIC ${kiero_SOURCE_DIR}
)

# Gotta patch it like this. For some reason it only works like this.
file(
  READ "${kiero_SOURCE_DIR}/kiero.h" KIERO_HEADER
)

# Patch D3D11 flag
string(REPLACE
    "#define KIERO_INCLUDE_D3D11  0"
    "#define KIERO_INCLUDE_D3D11  1"
    KIERO_HEADER
    "${KIERO_HEADER}"
)

# Patch MinHook flag
string(REPLACE
    "#define KIERO_USE_MINHOOK    0"
    "#define KIERO_USE_MINHOOK    1"
    KIERO_HEADER
    "${KIERO_HEADER}"
)

file(
  WRITE "${kiero_SOURCE_DIR}/kiero.h" "${KIERO_HEADER}"
)

# ------------------------
# ImGui
# ------------------------
FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG 934c6a5f5ef2355d6df25395d555cb71f790c4e9
)
FetchContent_MakeAvailable(imgui)

add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_win32.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_dx11.cpp
)

target_include_directories(imgui PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

target_link_libraries(
  imgui PUBLIC d3d11 dxgi d3dcompiler
)

# ------------------------
# Include directories
# ------------------------
include_directories(
    ${CMAKE_SOURCE_DIR}/Include
)

# ------------------------
# Sources
# ------------------------
file(GLOB_RECURSE PROJECT_SOURCES_CXX
    ${CMAKE_SOURCE_DIR}/Source/*.cpp
    ${CMAKE_SOURCE_DIR}/Source/*.h
)

file(GLOB_RECURSE PROJECT_SOURCES_HH
    ${CMAKE_SOURCE_DIR}/Include/*.hpp
    ${CMAKE_SOURCE_DIR}/Include/*.h
)

# ------------------------
# Target
# ------------------------
add_library(${CMAKE_PROJECT_NAME} SHARED
    ${PROJECT_SOURCES_CXX}
    ${PROJECT_SOURCES_HH}
)

# ------------------------
# Link libraries
# ------------------------
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE
    minhook
    kiero
    imgui
    d3d11.lib
    dxgi.lib
    d3dcompiler.lib
)

# ------------------------
# DLL Proxy
# ------------------------
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    "${CMAKE_SOURCE_DIR}/DllProxy.def"
)
