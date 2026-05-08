#include "feature_tracking/BinaryImage.hpp"
#include "feature_tracking/GridDetector.hpp"
#include "feature_tracking/ImageTransform.hpp"
#include "feature_tracking/NearestNeighborMatcher.hpp"

#include <gtest/gtest.h>

namespace ft::test {

// Verifies end-to-end behavior for translation, detection, and matching on a stable fixture.
TEST(PipelineRegression, TranslatedBinaryImageProducesExpectedFeatureCorrespondences) {
    const BinaryImage imageA{
        {
            {0, 1, 0, 0, 0},
            {0, 0, 0, 1, 0},
            {1, 0, 0, 0, 0},
            {0, 0, 1, 0, 0},
            {0, 0, 0, 0, 1}
        }
    };

    const BinaryImage imageB = ImageTransform::translate(imageA, 1, 0);

    const GridDetector detector;
    const Frame frameA = detector.detect(imageA);
    const Frame frameB = detector.detect(imageB);

    const NearestNeighborMatcher matcher;
    const MatchResult result = matcher.match(frameA, frameB, MatchOptions{1.1, true});

    // Regression guard: this full pipeline scenario fixes the expected behavior
    // across BinaryImage -> ImageTransform -> GridDetector -> Matcher.
    ASSERT_EQ(result.matches.size(), 4U);
    EXPECT_EQ(result.unmatchedA, 1U);
    EXPECT_DOUBLE_EQ(result.averageDistance(), 1.0);

    EXPECT_EQ(result.matches[0].indexA, 0U);
    EXPECT_EQ(result.matches[0].indexB, 0U);
    EXPECT_EQ(result.matches[1].indexA, 1U);
    EXPECT_EQ(result.matches[1].indexB, 1U);
    EXPECT_EQ(result.matches[2].indexA, 2U);
    EXPECT_EQ(result.matches[2].indexB, 2U);
    EXPECT_EQ(result.matches[3].indexA, 3U);
    EXPECT_EQ(result.matches[3].indexB, 3U);
}

// Verifies clipping during translation does not introduce false correspondences.
TEST(PipelineRegression, ClippedTranslationDoesNotCreateSpuriousMatches) {
    const BinaryImage imageA{
        {
            {1, 0, 0},
            {0, 1, 0},
            {0, 0, 1}
        }
    };

    const BinaryImage imageB = ImageTransform::translate(imageA, 1, 0);

    const GridDetector detector;
    const Frame frameA = detector.detect(imageA);
    const Frame frameB = detector.detect(imageB);

    const NearestNeighborMatcher matcher;
    const MatchResult result = matcher.match(frameA, frameB, MatchOptions{1.1, true});

    // The right-most feature is clipped during translation, so only two
    // correspondences are valid in the transformed frame.
    ASSERT_EQ(result.matches.size(), 2U);
    EXPECT_EQ(result.unmatchedA, 1U);
}

} // namespace ft::test
