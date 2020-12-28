add_executable (example1 example1.cpp)
target_include_directories (example1 PRIVATE ../../include)

set_target_properties(example1 PROPERTIES
        CXX_STANDARD 20
        CXX_EXTENSIONS OFF)
