#include "feature_tracking/GridDetector.hpp"

#include <gtest/gtest.h>

namespace ft::test {
// 
// Verifies that no features are detected when the image has only background pixels.
TEST(GridDetectorDetection, ReturnsEmptyFrameForAllBackgroundPixels) {
    const BinaryImage image{
        {
            {0, 0, 0},
            {0, 0, 0}
        }
    };

    const GridDetector detector;
    const Frame frame = detector.detect(image);

    EXPECT_TRUE(frame.empty());
}

// Verifies that one active pixel is detected as exactly one feature at its location.
TEST(GridDetectorDetection, DetectsSingleActivePixelAsFeature) {
    const BinaryImage image{
        {
            {0, 0, 0},
            {0, 1, 0},
            {0, 0, 0}
        }
    };

    const GridDetector detector;
    const Frame frame = detector.detect(image);

    ASSERT_EQ(frame.size(), 1U);
    EXPECT_EQ(frame[0], (Feature{1, 1}));
}

// Verifies that multiple active pixels are reported in deterministic scan order.
TEST(GridDetectorDetection, DetectsMultipleFeaturesInScanOrder) {
    const BinaryImage image{
        {
            {0, 1, 0},
            {1, 0, 1},
            {0, 0, 1}
        }
    };

    const GridDetector detector;
    const Frame frame = detector.detect(image);

    const Frame expected{
        Feature{1, 0},
        Feature{0, 1},
        Feature{2, 1},
        Feature{2, 2}
    };

    EXPECT_EQ(frame, expected);
}

// Verifies that features located on image borders and corners are still detected.
TEST(GridDetectorDetection, DetectsFeaturesAtImageBoundaries) {
    const BinaryImage image{
        {
            {1, 0, 1},
            {0, 0, 0},
            {1, 0, 1}
        }
    };

    const GridDetector detector;
    const Frame frame = detector.detect(image);

    const Frame expected{
        Feature{0, 0},
        Feature{2, 0},
        Feature{0, 2},
        Feature{2, 2}
    };

    EXPECT_EQ(frame, expected);
}

} // namespace ft::test
