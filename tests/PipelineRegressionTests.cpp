#include "feature_tracking/BinaryImage.hpp"
#include "feature_tracking/GridDetector.hpp"
#include "feature_tracking/ImageTransform.hpp"
#include "feature_tracking/NearestNeighborMatcher.hpp"

#include <chrono>
#include <cmath>
#include <iostream>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

namespace ft::test {
namespace {

BinaryImage makeSparseGridImage(
    std::size_t width,
    std::size_t height,
    const std::vector<std::pair<std::size_t, std::size_t>>& activePixels
) {
    BinaryImage::Grid grid(height, std::vector<int>(width, 0));

    for (const auto& [x, y] : activePixels) {
        grid[y][x] = 1;
    }

    return BinaryImage{grid};
}

std::vector<std::pair<std::size_t, std::size_t>> makeRegularFeaturePattern(
    std::size_t firstX,
    std::size_t firstY,
    std::size_t lastXInclusive,
    std::size_t lastYInclusive,
    std::size_t step
) {
    std::vector<std::pair<std::size_t, std::size_t>> pixels;

    for (std::size_t y = firstY; y <= lastYInclusive; y += step) {
        for (std::size_t x = firstX; x <= lastXInclusive; x += step) {
            pixels.emplace_back(x, y);
        }
    }

    return pixels;
}

MatchResult runPipeline(
    const BinaryImage& imageA,
    const BinaryImage& imageB,
    const MatchOptions& options
) {
    const GridDetector detector;
    const Frame frameA = detector.detect(imageA);
    const Frame frameB = detector.detect(imageB);

    const NearestNeighborMatcher matcher;
    return matcher.match(frameA, frameB, options);
}

} // namespace

/*
 * Regression guard:
 * Verifies the normal end-to-end pipeline behavior for a translated regular
 * feature pattern. This protects the integrated behavior of BinaryImage,
 * ImageTransform, GridDetector, and NearestNeighborMatcher.
 */
TEST(PipelineRegression, TranslatedRegularPatternProducesStableCorrespondences) {
    const auto activePixels = makeRegularFeaturePattern(
        1, 1,
        10, 10,
        3
    );

    const BinaryImage imageA = makeSparseGridImage(12, 12, activePixels);
    const BinaryImage imageB = ImageTransform::translate(imageA, 1, 1);

    const MatchResult result = runPipeline(imageA, imageB, MatchOptions{1.5, true});

    ASSERT_EQ(result.matches.size(), activePixels.size());
    EXPECT_EQ(result.unmatchedA, 0U);
    EXPECT_NEAR(result.averageDistance(), std::sqrt(2.0), 1e-9);

    for (const Match& match : result.matches) {
        EXPECT_EQ(match.indexA, match.indexB);
        EXPECT_NEAR(match.distance, std::sqrt(2.0), 1e-9);
    }
}

/*
 * Regression guard:
 * Repeats the full pipeline over several controlled translations. This makes
 * the regression suite broader than unit tests and validates that motion in
 * different directions preserves expected feature correspondences.
 */
TEST(PipelineRegression, MultipleControlledTranslationsPreserveFeatureCountAndDistance) {
    const auto activePixels = makeRegularFeaturePattern(
        2, 2,
        17, 17,
        3
    );

    const BinaryImage imageA = makeSparseGridImage(20, 20, activePixels);

    const std::vector<std::pair<int, int>> translations{
        {1, 0},
        {0, 1},
        {-1, 0},
        {0, -1},
        {2, 2}
    };

    for (const auto& [dx, dy] : translations) {
        SCOPED_TRACE("dx=" + std::to_string(dx) + ", dy=" + std::to_string(dy));

        const BinaryImage imageB = ImageTransform::translate(imageA, dx, dy);
        const double expectedDistance = std::sqrt(
            static_cast<double>(dx * dx + dy * dy)
        );

        const MatchResult result = runPipeline(imageA, imageB, MatchOptions{3.0, true});

        ASSERT_EQ(result.matches.size(), activePixels.size());
        EXPECT_EQ(result.unmatchedA, 0U);
        EXPECT_NEAR(result.averageDistance(), expectedDistance, 1e-9);
    }
}

/*
 * Regression guard:
 * Validates a clipping scenario. Some translated features leave the image
 * bounds, so the matcher must not create false correspondences for the lost
 * features.
 */
TEST(PipelineRegression, ClippedTranslationReducesMatchesWithoutCreatingFalseCorrespondences) {
    const std::vector<std::pair<std::size_t, std::size_t>> activePixels{
        {1, 1}, {5, 1}, {9, 1}, {13, 1}, {17, 1},
        {1, 3}, {5, 3}, {9, 3}, {13, 3}, {17, 3}
    };

    const BinaryImage imageA = makeSparseGridImage(19, 6, activePixels);
    const BinaryImage imageB = ImageTransform::translate(imageA, 2, 0);

    const MatchResult result = runPipeline(imageA, imageB, MatchOptions{2.1, true});

    ASSERT_EQ(result.matches.size(), 8U);
    EXPECT_EQ(result.unmatchedA, 2U);
    EXPECT_NEAR(result.averageDistance(), 2.0, 1e-9);
}

/*
 * Regression guard:
 * Uses a larger deterministic image than the module tests. This validates that
 * the complete pipeline remains stable when the number of detected features
 * increases.
 */
TEST(PipelineRegression, LargerSyntheticImageStressScenarioPreservesAllMatches) {
    const auto activePixels = makeRegularFeaturePattern(
        2, 2,
        38, 38,
        4
    );

    const BinaryImage imageA = makeSparseGridImage(40, 40, activePixels);
    const BinaryImage imageB = ImageTransform::translate(imageA, 1, 1);

    const MatchResult result = runPipeline(imageA, imageB, MatchOptions{1.5, true});

    ASSERT_EQ(result.matches.size(), activePixels.size());
    EXPECT_EQ(result.unmatchedA, 0U);
    EXPECT_NEAR(result.averageDistance(), std::sqrt(2.0), 1e-9);
}

/*
 * Heavy regression scenario:
 * Executes the complete pipeline repeatedly on a larger synthetic image and
 * several motion cases. This test intentionally belongs to the full suite,
 * not the fast push suite, and gives the CI prioritization strategy a real
 * execution-cost difference to discuss in the report.
 */
TEST(PipelineRegression, LargeSyntheticImageRepeatedMotionStressScenario) {
    const auto activePixels = makeRegularFeaturePattern(
        2, 2,
        242, 242,
        5
    );

    const BinaryImage imageA = makeSparseGridImage(250, 250, activePixels);

    const std::vector<std::pair<int, int>> translations{
        {1, 1},
        {2, 0},
        {0, 2},
        {-1, 1},
        {1, -1},
        {3, 0},
        {0, 3},
        {-2, 0},
        {0, -2},
        {2, 2}
    };

    const auto start = std::chrono::steady_clock::now();

    for (const auto& [dx, dy] : translations) {
        SCOPED_TRACE("large regression dx=" + std::to_string(dx) +
                     ", dy=" + std::to_string(dy));

        const BinaryImage imageB = ImageTransform::translate(imageA, dx, dy);
        const double expectedDistance = std::sqrt(
            static_cast<double>(dx * dx + dy * dy)
        );

        const MatchResult result = runPipeline(imageA, imageB, MatchOptions{4.5, true});

        ASSERT_EQ(result.matches.size(), activePixels.size());
        EXPECT_EQ(result.unmatchedA, 0U);
        EXPECT_NEAR(result.averageDistance(), expectedDistance, 1e-9);
    }

    const auto end = std::chrono::steady_clock::now();
    const auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(
        end - start
    ).count();

    std::cout << "[REGRESSION STRESS] active_features=" << activePixels.size()
              << ", translations=" << translations.size()
              << ", elapsed_ms=" << elapsedMs << '\n';
}

} // namespace ft::test