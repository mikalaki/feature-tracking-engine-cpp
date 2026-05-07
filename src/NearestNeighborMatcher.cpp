#include "feature_tracking/NearestNeighborMatcher.hpp"

#include <cmath>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <vector>

namespace ft {

double MatchResult::averageDistance() const noexcept {
    if (matches.empty()) {
        return 0.0;
    }

    const double total = std::accumulate(
        matches.begin(),
        matches.end(),
        0.0,
        [](double sum, const Match& match) {
            return sum + match.distance;
        }
    );

    return total / static_cast<double>(matches.size());
}

double NearestNeighborMatcher::euclideanDistance(
    const Feature& a,
    const Feature& b
) noexcept {
    const auto dx = static_cast<double>(a.x - b.x);
    const auto dy = static_cast<double>(a.y - b.y);

    return std::sqrt(dx * dx + dy * dy);
}

MatchResult NearestNeighborMatcher::match(
    const Frame& frameA,
    const Frame& frameB,
    const MatchOptions& options
) const {
    if (options.maxDistance < 0.0) {
        throw std::invalid_argument(
            "NearestNeighborMatcher: maxDistance must be non-negative"
        );
    }

    MatchResult result;

    if (frameA.empty()) {
        return result;
    }

    if (frameB.empty()) {
        result.unmatchedA = frameA.size();
        return result;
    }

    std::vector<bool> usedB(frameB.size(), false);

    for (std::size_t i = 0; i < frameA.size(); ++i) {
        double bestDistance = std::numeric_limits<double>::max();
        std::size_t bestIndexB = frameB.size();
        bool foundCandidate = false;

        for (std::size_t j = 0; j < frameB.size(); ++j) {
            if (options.enforceUniqueMatches && usedB[j]) {
                continue;
            }

            const double distance = euclideanDistance(frameA[i], frameB[j]);

            if (distance < bestDistance) {
                bestDistance = distance;
                bestIndexB = j;
                foundCandidate = true;
            }
        }

        if (foundCandidate && bestDistance <= options.maxDistance) {
            result.matches.push_back(
                Match{
                    i,
                    bestIndexB,
                    bestDistance
                }
            );

            if (options.enforceUniqueMatches) {
                usedB[bestIndexB] = true;
            }
        } else {
            ++result.unmatchedA;
        }
    }

    return result;
}

} // namespace ft
