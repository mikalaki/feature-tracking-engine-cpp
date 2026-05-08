#!/usr/bin/env bash
set -euo pipefail

# Module-based regression test selection.
#
# Input:  a list of changed files, usually produced by git diff --name-only.
# Output: a CTest label regular expression suitable for: ctest -L "$(...)".
#
# Selection rule:
# - run tests for directly changed modules;
# - always include regression/smoke tests for changed production code;
# - run the full labeled suite when build/test infrastructure or broad headers change;
# - run only fast regression checks for documentation-only changes.

if [[ "$#" -eq 0 ]]; then
    echo "fast|regression|smoke"
    exit 0
fi

needs_full_suite=false
declare -a labels=("regression" "smoke")

add_label() {
    local label="$1"
    for existing in "${labels[@]}"; do
        if [[ "$existing" == "$label" ]]; then
            return
        fi
    done
    labels+=("$label")
}

for changed_file in "$@"; do
    case "$changed_file" in
        src/BinaryImage.cpp|include/feature_tracking/BinaryImage.hpp)
            add_label "binary_image"
            ;;
        src/GridDetector.cpp|include/feature_tracking/GridDetector.hpp)
            add_label "grid_detector"
            ;;
        src/ImageTransform.cpp|include/feature_tracking/ImageTransform.hpp)
            add_label "image_transform"
            ;;
        src/NearestNeighborMatcher.cpp|include/feature_tracking/NearestNeighborMatcher.hpp)
            add_label "matcher"
            add_label "bva"
            ;;
        include/feature_tracking/Feature.hpp|include/feature_tracking/Frame.hpp)
            # Shared feature/frame abstractions affect detector and matcher behavior.
            add_label "grid_detector"
            add_label "matcher"
            add_label "pipeline"
            ;;
        app/*)
            add_label "smoke"
            ;;
        tests/*)
            # Test changes should validate the full test suite.
            needs_full_suite=true
            ;;
        CMakeLists.txt|tests/CMakeLists.txt|cmake/*|scripts/*|.github/workflows/*)
            # Build, selection, or CI changes can affect all test execution.
            needs_full_suite=true
            ;;
        README.md|docs/*)
            # Documentation-only changes do not require the full suite.
            ;;
        *)
            # Unknown changes are treated conservatively.
            needs_full_suite=true
            ;;
    esac
done

if [[ "$needs_full_suite" == true ]]; then
    echo ".*"
else
    IFS='|'
    echo "${labels[*]}"
fi
