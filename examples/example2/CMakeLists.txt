add_executable (example2 example2.cpp user1.cpp user2.cpp user3.cpp)
target_include_directories (example2 PRIVATE ../../include)

set_target_properties(example2 PROPERTIES
        CXX_STANDARD 20
        CXX_EXTENSIONS OFF)
