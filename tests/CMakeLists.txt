add_executable (tests test-main.cpp tests.cpp)
target_include_directories (tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include/)

set_target_properties(tests PROPERTIES
        CXX_STANDARD 20
        CXX_EXTENSIONS OFF)

enable_testing()
add_test(MyTest tests)