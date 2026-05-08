include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)

# Required for Windows/MSVC users; harmless on Linux CI.
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

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

    # The CI pipeline uses CTest labels for speed-based prioritization:
    # - fast: cheap module-level checks run on every push.
    # - full/regression: heavier end-to-end checks run on pull requests,
    #   nightly/manual workflows, or when the full suite is explicitly requested.
    #
    # Registering one CTest test per GoogleTest executable keeps labels stable
    # across CMake/GoogleTest versions while GoogleTest still reports the
    # individual test cases inside each executable.
    add_test(NAME ${target_name} COMMAND ${target_name})
    set_tests_properties(${target_name}
        PROPERTIES
            LABELS "${labels}"
    )
endfunction()

add_feature_tracking_test(
    binary_image_tests
    BinaryImageTests.cpp
    "fast;unit"
)

add_feature_tracking_test(
    grid_detector_tests
    GridDetectorTests.cpp
    "fast;unit"
)

add_feature_tracking_test(
    image_transform_tests
    ImageTransformTests.cpp
    "fast;unit"
)

add_feature_tracking_test(
    nearest_neighbor_matcher_tests
    NearestNeighborMatcherTests.cpp
    "fast;unit;bva"
)

add_feature_tracking_test(
    pipeline_regression_tests
    PipelineRegressionTests.cpp
    "full;integration;regression"
)
