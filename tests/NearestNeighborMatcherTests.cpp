#include "feature_tracking/NearestNeighborMatcher.hpp"

#include <gtest/gtest.h>

#include <cmath>
#include <stdexcept>

namespace ft::test {

// Verifies the Euclidean distance helper with a known 3-4-5 triangle.
TEST(NearestNeighborMatcherDistance, ComputesEuclideanDistance) {
    const Feature a{0, 0};
    const Feature b{3, 4};

    EXPECT_DOUBLE_EQ(NearestNeighborMatcher::euclideanDistance(a, b), 5.0);
}

// Verifies one-to-one perfect correspondences are matched with zero unmatched features.
TEST(NearestNeighborMatcherMatching, MatchesPerfectCorrespondences) {
    const Frame frameA{Feature{0, 0}, Feature{2, 2}};
    const Frame frameB{Feature{0, 0}, Feature{2, 2}};

    const NearestNeighborMatcher matcher;
    const MatchResult result = matcher.match(frameA, frameB, MatchOptions{0.0, true});

    ASSERT_EQ(result.matches.size(), 2U);
    EXPECT_EQ(result.matches[0].indexA, 0U);
    EXPECT_EQ(result.matches[0].indexB, 0U);
    EXPECT_EQ(result.matches[1].indexA, 1U);
    EXPECT_EQ(result.matches[1].indexB, 1U);
    EXPECT_EQ(result.unmatchedA, 0U);
}

// Verifies that negative distance thresholds are rejected as invalid options.
TEST(NearestNeighborMatcherValidation, ThrowsForNegativeThreshold) {
    const Frame frameA{Feature{0, 0}};
    const Frame frameB{Feature{0, 0}};

    const NearestNeighborMatcher matcher;

    EXPECT_THROW(
        matcher.match(frameA, frameB, MatchOptions{-0.1, true}),
        std::invalid_argument
    );
}

// Verifies empty source input yields no matches and no unmatched source count.
TEST(NearestNeighborMatcherEmptyInputs, EmptySourceFrameProducesNoMatchesAndNoUnmatchedSourceFeatures) {
    const Frame frameA{};
    const Frame frameB{Feature{0, 0}};

    const NearestNeighborMatcher matcher;
    const MatchResult result = matcher.match(frameA, frameB, MatchOptions{1.0, true});

    EXPECT_TRUE(result.matches.empty());
    EXPECT_EQ(result.unmatchedA, 0U);
}

// Verifies empty target input causes every source feature to be unmatched.
TEST(NearestNeighborMatcherEmptyInputs, EmptyTargetFrameMakesAllSourceFeaturesUnmatched) {
    const Frame frameA{Feature{0, 0}, Feature{1, 1}};
    const Frame frameB{};

    const NearestNeighborMatcher matcher;
    const MatchResult result = matcher.match(frameA, frameB, MatchOptions{1.0, true});

    EXPECT_TRUE(result.matches.empty());
    EXPECT_EQ(result.unmatchedA, frameA.size());
}

// Verifies unique matching policy prevents assigning one target to multiple sources.
TEST(NearestNeighborMatcherUniquePolicy, EnforcedUniqueMatchesPreventReusingTargetFeature) {
    const Frame frameA{Feature{0, 0}, Feature{0, 0}};
    const Frame frameB{Feature{0, 0}};

    const NearestNeighborMatcher matcher;
    const MatchResult result = matcher.match(frameA, frameB, MatchOptions{0.0, true});

    ASSERT_EQ(result.matches.size(), 1U);
    EXPECT_EQ(result.unmatchedA, 1U);
}

// Verifies disabling uniqueness allows multiple sources to reuse the same target.
TEST(NearestNeighborMatcherUniquePolicy, DisabledUniqueMatchesAllowReusingTargetFeature) {
    const Frame frameA{Feature{0, 0}, Feature{0, 0}};
    const Frame frameB{Feature{0, 0}};

    const NearestNeighborMatcher matcher;
    const MatchResult result = matcher.match(frameA, frameB, MatchOptions{0.0, false});

    ASSERT_EQ(result.matches.size(), 2U);
    EXPECT_EQ(result.matches[0].indexB, 0U);
    EXPECT_EQ(result.matches[1].indexB, 0U);
    EXPECT_EQ(result.unmatchedA, 0U);
}

// Verifies average distance defaults to zero when there are no accepted matches.
TEST(NearestNeighborMatcherStatistics, AverageDistanceIsZeroWhenNoMatchesExist) {
    const MatchResult result{};

    EXPECT_DOUBLE_EQ(result.averageDistance(), 0.0);
}

// Verifies average distance is computed from accepted match distances only.
TEST(NearestNeighborMatcherStatistics, AverageDistanceUsesAcceptedMatchesOnly) {
    MatchResult result;
    result.matches = {
        Match{0, 0, 1.0},
        Match{1, 1, 3.0}
    };

    EXPECT_DOUBLE_EQ(result.averageDistance(), 2.0);
}

// BVA: distance just below the maxDistance boundary must be accepted.
// Here the geometric distance is exactly 1.0 and the threshold is 1.01.
TEST(NearestNeighborMatcherThresholdBVA, AcceptsDistanceJustBelowThreshold) {
    const Frame frameA{Feature{0, 0}};
    const Frame frameB{Feature{1, 0}};

    const NearestNeighborMatcher matcher;
    const MatchResult result = matcher.match(frameA, frameB, MatchOptions{1.01, true});

    ASSERT_EQ(result.matches.size(), 1U);
    EXPECT_DOUBLE_EQ(result.matches[0].distance, 1.0);
    EXPECT_EQ(result.unmatchedA, 0U);
}

// BVA: distance exactly equal to maxDistance must be accepted because
// the implementation uses "<= maxDistance" as the acceptance condition.
TEST(NearestNeighborMatcherThresholdBVA, AcceptsDistanceExactlyAtThreshold) {
    const Frame frameA{Feature{0, 0}};
    const Frame frameB{Feature{1, 0}};

    const NearestNeighborMatcher matcher;
    const MatchResult result = matcher.match(frameA, frameB, MatchOptions{1.0, true});

    ASSERT_EQ(result.matches.size(), 1U);
    EXPECT_DOUBLE_EQ(result.matches[0].distance, 1.0);
    EXPECT_EQ(result.unmatchedA, 0U);
}

// BVA: distance just above the maxDistance boundary must be rejected.
// This guards the threshold comparison against accidental '<'/'<=' mistakes.
TEST(NearestNeighborMatcherThresholdBVA, RejectsDistanceJustAboveThreshold) {
    const Frame frameA{Feature{0, 0}};
    const Frame frameB{Feature{1, 0}};

    const NearestNeighborMatcher matcher;
    const MatchResult result = matcher.match(frameA, frameB, MatchOptions{0.99, true});

    EXPECT_TRUE(result.matches.empty());
    EXPECT_EQ(result.unmatchedA, 1U);
}

} // namespace ft::test
