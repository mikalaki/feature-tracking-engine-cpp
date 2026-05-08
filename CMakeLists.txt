cmake_minimum_required(VERSION 3.16)

project(feature_tracking_engine_cpp
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(feature_tracking_engine
    src/BinaryImage.cpp
    src/GridDetector.cpp
    src/ImageTransform.cpp
    src/NearestNeighborMatcher.cpp
)

target_include_directories(feature_tracking_engine
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    target_compile_options(feature_tracking_engine
        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
    )
endif()

add_executable(feature_tracking_demo
    app/main.cpp
)

target_link_libraries(feature_tracking_demo
    PRIVATE
        feature_tracking_engine
)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    target_compile_options(feature_tracking_demo
        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
    )
endif()

include(CTest)

if(BUILD_TESTING)
    add_subdirectory(tests)

    add_test(NAME demo_smoke COMMAND feature_tracking_demo)
    set_tests_properties(demo_smoke
        PROPERTIES
            LABELS "fast;smoke"
    )
endif()
