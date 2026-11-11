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
find_package(TBB REQUIRED)

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
