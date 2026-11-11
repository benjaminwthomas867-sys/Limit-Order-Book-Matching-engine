cmake_minimum_required(VERSION 3.16)
project(LimitOrderBookMatchingEngine LANGUAGES CXX)

# std::format (used throughout) requires C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT WIN32)
    message(WARNING "This project uses Winsock (winsock2.h) and is Windows-only. "
                    "Build with MSVC or MinGW-w64.")
endif()

# ---------------------------------------------------------------------------
# Intel oneTBB (Exchange.cpp includes "include/oneapi/tbb/...")
#   Option A: a local copy of oneTBB's headers in ./include/oneapi/tbb
#             plus the library, e.g. -DTBB_ROOT=C:/path/to/oneTBB
#   Option B: an installed oneTBB found via find_package (vcpkg: `vcpkg install tbb`)
# ---------------------------------------------------------------------------

# Default to the oneAPI install location; override with -DTBB_DIR=...
if(NOT TBB_DIR AND NOT DEFINED ENV{TBB_DIR})
    set(_oneapi_tbb "C:/Program Files (x86)/Intel/oneAPI/tbb/2023.2/lib/cmake/tbb")
    if(EXISTS "${_oneapi_tbb}/TBBConfig.cmake")
        set(TBB_DIR "${_oneapi_tbb}" CACHE PATH "Folder containing TBBConfig.cmake")
    endif()
endif()

find_package(TBB REQUIRED)

# TBB install root = <root>/lib/cmake/tbb -> go up three levels.
# Adding it as an include path lets Exchange.cpp's existing
# #include "include/oneapi/tbb/..." lines resolve without editing the source.
get_filename_component(TBB_ROOT_DIR "${TBB_DIR}/../../.." ABSOLUTE)
message(STATUS "TBB found via ${TBB_DIR} (root: ${TBB_ROOT_DIR})")

# ---------------------------------------------------------------------------
# Shared sources: order book + matching engine + helpers
# ---------------------------------------------------------------------------
add_library(lob_core STATIC
    LOB.cpp
    Matching_engine.cpp
    "Universal functions.cpp"
)
target_include_directories(lob_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# ---------------------------------------------------------------------------
# Exchange (server, listens on port 8080)
# ---------------------------------------------------------------------------
add_executable(Exchange Exchange.cpp)
target_include_directories(Exchange PRIVATE "${TBB_ROOT_DIR}")
target_link_libraries(Exchange PRIVATE lob_core TBB::tbb)
if(WIN32)
    target_link_libraries(Exchange PRIVATE ws2_32)
endif()

# ---------------------------------------------------------------------------
# Person (client, connects to 127.0.0.1:8080)
# ---------------------------------------------------------------------------
add_executable(Person Person.cpp "Universal functions.cpp")
target_include_directories(Person PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
if(WIN32)
    target_link_libraries(Person PRIVATE ws2_32)
endif()

if(MSVC)
    target_compile_options(lob_core PRIVATE /EHsc /W3)
    target_compile_options(Exchange PRIVATE /EHsc /W3)
    target_compile_options(Person   PRIVATE /EHsc /W3)
endif()

# Copy the TBB runtime DLL next to the executables on Windows (if available)
if(WIN32 AND TARGET TBB::tbb)
    add_custom_command(TARGET Exchange POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:TBB::tbb> $<TARGET_FILE_DIR:Exchange>
        COMMENT "Copying TBB runtime next to Exchange")
endif()

