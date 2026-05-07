#pragma once

#include "feature_tracking/BinaryImage.hpp"
#include "feature_tracking/Frame.hpp"

namespace ft {

/**
 * @brief Detects features from a synthetic binary image/grid.
 *
 * Detection rule:
 * - every pixel with value 1 becomes a Feature(x, y)
 * - every pixel with value 0 is ignored
 *
 * This intentionally simple detector makes the SUT deterministic and suitable
 * for controlled testing.
 */
class GridDetector {
public:
    /**
     * @brief Extract feature points from a binary image.
     * @param image Input binary image.
     * @return Frame containing one Feature for each active pixel.
     */
    [[nodiscard]] Frame detect(const BinaryImage& image) const;
};

} // namespace ft
