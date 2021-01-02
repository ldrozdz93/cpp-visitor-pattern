add_executable (benchmark benchmark-main.cpp benchmark.cpp)
target_include_directories (benchmark PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/picobench/include/)
target_include_directories (benchmark PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include/)

set_target_properties(benchmark PROPERTIES
        CXX_STANDARD 20
        CXX_EXTENSIONS OFF)
