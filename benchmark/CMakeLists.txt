add_executable (vstor-benchmark benchmark-main.cpp benchmark.cpp)
target_include_directories (vstor-benchmark PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include/)

set_target_properties(vstor-benchmark PROPERTIES
        CXX_STANDARD 20
        CXX_EXTENSIONS OFF)

set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Suppressing benchmark's tests" FORCE)
add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/google_benchmark/)
target_link_libraries(vstor-benchmark PRIVATE benchmark::benchmark)
