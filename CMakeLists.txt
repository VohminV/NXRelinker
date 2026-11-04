cmake_minimum_required(VERSION 3.24)

project(
    NXRelinker
    VERSION 0.1.0
    DESCRIPTION "PC-side Nintendo Switch binary analysis, relinking and recompilation toolkit"
    LANGUAGES CXX
)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

option(NXRELINKER_BUILD_TESTS "Build the NXRelinker test suite" ON)
option(NXRELINKER_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" ON)
option(NXRELINKER_ENABLE_SANITIZERS "Enable AddressSanitizer and UndefinedBehaviorSanitizer" OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(CompilerWarnings)
include(Sanitizers)
nxrelinker_configure_sanitizers()

add_library(nxrelinker STATIC
    src/binary/elf.cpp
    src/binary/format_detector.cpp
    src/binary/inspection.cpp
    src/binary/nro.cpp
    src/binary/nso.cpp
    src/core/hex.cpp
    src/diagnostics/diagnostic.cpp
    src/diagnostics/json_writer.cpp
)

target_include_directories(nxrelinker
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(nxrelinker PUBLIC cxx_std_20)
nxrelinker_apply_warnings(nxrelinker)

if(MSVC)
    target_compile_definitions(nxrelinker PUBLIC _CRT_SECURE_NO_WARNINGS NOMINMAX WIN32_LEAN_AND_MEAN)
endif()

add_executable(nxinspect tools/nxinspect/main.cpp)
target_compile_features(nxinspect PRIVATE cxx_std_20)
target_link_libraries(nxinspect PRIVATE nxrelinker)
target_compile_definitions(nxinspect PRIVATE NXRELINKER_VERSION_STRING="${PROJECT_VERSION}")
nxrelinker_apply_warnings(nxinspect)

if(NXRELINKER_BUILD_TESTS)
    include(CTest)
    enable_testing()
    add_subdirectory(tests)
endif()

install(TARGETS nxrelinker nxinspect
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)
install(DIRECTORY include/nxrelinker DESTINATION include)
