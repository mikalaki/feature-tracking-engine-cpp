#include "feature_tracking/BinaryImage.hpp"

#include <gtest/gtest.h>

#include <stdexcept>

namespace ft::test {

// Verifies that construction from a valid grid preserves shape and stored values.
TEST(BinaryImageConstruction, StoresValidGridDimensionsAndPixelValues) {
    const BinaryImage image{
        {
            {0, 1, 0},
            {1, 0, 1}
        }
    };

    EXPECT_EQ(image.width(), 3U);
    EXPECT_EQ(image.height(), 2U);
    EXPECT_EQ(image.at(1, 0), 1);
    EXPECT_EQ(image.at(1, 1), 0);
}

// Verifies that an empty grid is rejected as invalid input.
TEST(BinaryImageValidation, ThrowsWhenGridIsEmpty) {
    EXPECT_THROW(BinaryImage{BinaryImage::Grid{}}, std::invalid_argument);
}

// Verifies that rows with zero columns are rejected.
TEST(BinaryImageValidation, ThrowsWhenGridHasZeroWidth) {
    EXPECT_THROW(BinaryImage{BinaryImage::Grid{{}}}, std::invalid_argument);
}

// Verifies that non-rectangular grids (inconsistent row sizes) are rejected.
TEST(BinaryImageValidation, ThrowsWhenGridIsNotRectangular) {
    EXPECT_THROW(
        BinaryImage({
            {0, 1, 0},
            {1, 0}
        }),
        std::invalid_argument
    );
}

// Verifies that only binary pixel values (0 or 1) are accepted.
TEST(BinaryImageValidation, ThrowsWhenGridContainsNonBinaryValues) {
    EXPECT_THROW(
        BinaryImage({
            {0, 1},
            {2, 0}
        }),
        std::invalid_argument
    );
}

// Verifies read access for all corner coordinates on the image boundary.
TEST(BinaryImageAccess, ReadsBoundaryCoordinates) {
    const BinaryImage image{
        {
            {1, 0, 1},
            {0, 0, 0},
            {1, 0, 1}
        }
    };

    EXPECT_EQ(image.at(0, 0), 1);
    EXPECT_EQ(image.at(2, 0), 1);
    EXPECT_EQ(image.at(0, 2), 1);
    EXPECT_EQ(image.at(2, 2), 1);
}

// Verifies that out-of-bounds coordinate reads throw a range error.
TEST(BinaryImageAccess, ThrowsWhenCoordinateIsOutOfBounds) {
    const BinaryImage image{
        {
            {0, 1},
            {1, 0}
        }
    };

    EXPECT_THROW(image.at(2, 0), std::out_of_range);
    EXPECT_THROW(image.at(0, 2), std::out_of_range);
}

} // namespace ft::test
