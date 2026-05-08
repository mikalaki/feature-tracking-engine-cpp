#include "feature_tracking/GridDetector.hpp"
#include "feature_tracking/ImageTransform.hpp"

#include <gtest/gtest.h>

namespace ft::test {

namespace {

Frame detectFeatures(const BinaryImage& image) {
    const GridDetector detector;
    return detector.detect(image);
}

} // namespace

// Verifies that a zero translation leaves the binary image unchanged.
TEST(ImageTransformTranslation, ZeroTranslationPreservesActivePixels) {
    const BinaryImage image{
        {
            {0, 1, 0},
            {1, 0, 1}
        }
    };

    const BinaryImage translated = ImageTransform::translate(image, 0, 0);

    EXPECT_EQ(translated.data(), image.data());
}

// Verifies that positive translation offsets shift features to the expected coordinates.
TEST(ImageTransformTranslation, PositiveTranslationMovesFeaturesWithinBounds) {
    const BinaryImage image{
        {
            {1, 0, 0},
            {0, 1, 0},
            {0, 0, 0}
        }
    };

    const BinaryImage translated = ImageTransform::translate(image, 1, 1);

    const Frame expected{
        Feature{1, 1},
        Feature{2, 2}
    };

    EXPECT_EQ(detectFeatures(translated), expected);
}

// Verifies that negative translation offsets shift features back toward the origin.
TEST(ImageTransformTranslation, NegativeTranslationMovesFeaturesWithinBounds) {
    const BinaryImage image{
        {
            {0, 0, 0},
            {0, 1, 0},
            {0, 0, 1}
        }
    };

    const BinaryImage translated = ImageTransform::translate(image, -1, -1);

    const Frame expected{
        Feature{0, 0},
        Feature{1, 1}
    };

    EXPECT_EQ(detectFeatures(translated), expected);
}

// Verifies that translated features leaving image bounds are clipped (discarded).
TEST(ImageTransformTranslation, ClipsFeaturesThatMoveOutsideImageBounds) {
    const BinaryImage image{
        {
            {1, 0, 0},
            {0, 1, 0},
            {0, 0, 1}
        }
    };

    const BinaryImage translated = ImageTransform::translate(image, 1, 0);

    const Frame expected{
        Feature{1, 0},
        Feature{2, 1}
    };

    EXPECT_EQ(detectFeatures(translated), expected);
}

// Verifies that very large translations can move all features out of frame.
TEST(ImageTransformTranslation, LargeTranslationProducesImageWithoutFeatures) {
    const BinaryImage image{
        {
            {1, 0},
            {0, 1}
        }
    };

    const BinaryImage translated = ImageTransform::translate(image, 10, 10);

    EXPECT_TRUE(detectFeatures(translated).empty());
}

} // namespace ft::test
