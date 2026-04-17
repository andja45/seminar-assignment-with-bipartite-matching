add_library(core STATIC)

target_include_directories(core PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_sources(core PRIVATE
        graph/AllocationGraph.cpp
        rounds/RoundHistory.cpp
        matching/SimpleMatching.cpp
        matching/HopcroftKarp.cpp
        cover/KonigCover.cpp
        cover/CriticalityAnalyzer.cpp
)

add_executable(core_demo main.cpp)
target_link_libraries(core_demo PRIVATE core)

target_compile_options(core PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic -Werror>
)
