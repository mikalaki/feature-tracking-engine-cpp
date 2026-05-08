#include "feature_tracking/NearestNeighborMatcher.hpp"
#include "feature_tracking/GridDetector.hpp"
#include "feature_tracking/BinaryImage.hpp"

#include <cassert>
#include <cmath>
#include <stdexcept>

using namespace ft;

static bool near(double a, double b, double eps = 1e-9) {
    return std::abs(a - b) < eps;
}

int main() {
    NearestNeighborMatcher matcher;

    // Boundary threshold: distance exactly equal to maxDistance must be accepted.
    {
        Frame a{{0, 0}};
        Frame b{{1, 0}};
        auto result = matcher.match(a, b, MatchOptions{1.0, true});
        assert(result.matches.size() == 1);
        assert(result.unmatchedA == 0);
        assert(near(result.matches[0].distance, 1.0));
    }

    // Rejection beyond threshold and unmatched count.
    {
        Frame a{{0, 0}, {10, 10}};
        Frame b{{1, 0}};
        auto result = matcher.match(a, b, MatchOptions{1.1, true});
        assert(result.matches.size() == 1);
        assert(result.unmatchedA == 1);
    }

    // Unique matching: one B feature cannot be reused.
    {
        Frame a{{0, 0}, {0, 0}};
        Frame b{{0, 0}};
        auto result = matcher.match(a, b, MatchOptions{0.0, true});
        assert(result.matches.size() == 1);
        assert(result.unmatchedA == 1);
    }

    // Non-unique matching: same B feature may be reused when configured.
    {
        Frame a{{0, 0}, {0, 0}};
        Frame b{{0, 0}};
        auto result = matcher.match(a, b, MatchOptions{0.0, false});
        assert(result.matches.size() == 2);
        assert(result.unmatchedA == 0);
    }

    // Negative threshold must be rejected.
    {
        bool thrown = false;
        try {
            (void)matcher.match(Frame{{0, 0}}, Frame{{0, 0}}, MatchOptions{-1.0, true});
        } catch (const std::invalid_argument&) {
            thrown = true;
        }
        assert(thrown);
    }

    // Detector + matcher integration with a shifted binary image.
    {
        const BinaryImage imgA{{
            {1,0,0},
            {0,1,0},
            {0,0,1}
        }};
        const BinaryImage imgB{{
            {0,1,0},
            {0,0,1},
            {0,0,0}
        }};
        GridDetector detector;
        Frame frameA = detector.detect(imgA);
        Frame frameB = detector.detect(imgB);
        auto result = matcher.match(frameA, frameB, MatchOptions{1.0, true});
        assert(result.matches.size() == 2); // last feature moved out of image
        assert(result.unmatchedA == 1);
    }

    return 0;
}
