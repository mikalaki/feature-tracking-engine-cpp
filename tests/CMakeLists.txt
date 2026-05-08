include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)

# Required for Windows/MSVC users; harmless on Linux CI.
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)

function(add_feature_tracking_test target_name source_file labels)
    add_executable(${target_name}
        ${source_file}
    )

    target_link_libraries(${target_name}
        PRIVATE
            feature_tracking_engine
            GTest::gtest_main
    )

    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
        target_compile_options(${target_name}
            PRIVATE
                -Wall
                -Wextra
                -Wpedantic
        )
    endif()

    gtest_discover_tests(${target_name}
        DISCOVERY_TIMEOUT 30
        PROPERTIES
            LABELS "${labels}"
    )
endfunction()

add_feature_tracking_test(binary_image_tests
    BinaryImageTests.cpp
    "unit;fast;binary_image"
)

add_feature_tracking_test(grid_detector_tests
    GridDetectorTests.cpp
    "unit;fast;grid_detector"
)

add_feature_tracking_test(image_transform_tests
    ImageTransformTests.cpp
    "unit;fast;image_transform"
)

add_feature_tracking_test(nearest_neighbor_matcher_tests
    NearestNeighborMatcherTests.cpp
    "unit;fast;matcher;bva"
)

add_feature_tracking_test(pipeline_regression_tests
    PipelineRegressionTests.cpp
    "integration;regression;pipeline"
)
