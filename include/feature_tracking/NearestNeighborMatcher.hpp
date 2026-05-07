#pragma once

#include "feature_tracking/Frame.hpp"

#include <cstddef>
#include <vector>

namespace ft {

/**
 * @brief Represents a correspondence between one feature in Frame A and one
 * feature in Frame B.
 */
struct Match {
    std::size_t indexA{};
    std::size_t indexB{};
    double distance{};
};

/**
 * @brief Configuration for the nearest-neighbor matcher.
 */
struct MatchOptions {
    /**
     * @brief Maximum accepted Euclidean distance between matched features.
     */
    double maxDistance{1.5};

    /**
     * @brief If true, one feature in Frame B can be used at most once.
     *
     * If false, multiple features from Frame A may match the same feature in
     * Frame B. Keeping this configurable supports richer tests.
     */
    bool enforceUniqueMatches{true};
};

/**
 * @brief Result of a matching operation.
 */
struct MatchResult {
    std::vector<Match> matches;

    /**
     * @brief Number of features in Frame A that did not receive a valid match.
     */
    std::size_t unmatchedA{};

    /**
     * @brief Average distance of accepted matches.
     *
     * Returns 0.0 when no matches exist.
     */
    [[nodiscard]] double averageDistance() const noexcept;
};

/**
 * @brief Brute-force nearest-neighbor matcher.
 *
 * For each feature in Frame A, the matcher compares it against all features
 * in Frame B, keeps the closest candidate, and accepts it only if the distance
 * is below the configured threshold.
 *
 * Complexity: O(N * M), where N is the number of features in Frame A and M is
 * the number of features in Frame B.
 */
class NearestNeighborMatcher {
public:
    /**
     * @brief Match two frames using nearest-neighbor search.
     * @param frameA Source frame.
     * @param frameB Target frame.
     * @param options Matching configuration.
     * @return MatchResult containing accepted correspondences.
     * @throws std::invalid_argument if maxDistance is negative.
     */
    [[nodiscard]] MatchResult match(
        const Frame& frameA,
        const Frame& frameB,
        const MatchOptions& options
    ) const;

    /**
     * @brief Compute Euclidean distance between two feature points.
     */
    [[nodiscard]] static double euclideanDistance(
        const Feature& a,
        const Feature& b
    ) noexcept;
};

} // namespace ft
