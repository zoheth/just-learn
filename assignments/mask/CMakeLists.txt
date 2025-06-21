add_executable(mask
        impl.h
        tiled_matrix.h
        ref_impl.cpp)

target_sources(mask PRIVATE
    tests.cpp
)

target_include_directories(mask PRIVATE
    ${PROJECT_SOURCE_DIR}
)

target_link_libraries(mask PRIVATE GTest::gtest_main)

include(GoogleTest)

gtest_discover_tests(mask)