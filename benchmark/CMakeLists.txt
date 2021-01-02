add_executable (tests_odwyer benchmark-main.cpp benchmark.cpp)
target_include_directories (tests_odwyer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include/)

set_target_properties(tests_odwyer PROPERTIES
        CXX_STANDARD 20
        CXX_EXTENSIONS OFF)

enable_testing()
add_test(MyTest tests_odwyer)