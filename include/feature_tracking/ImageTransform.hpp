#pragma once

#include "feature_tracking/BinaryImage.hpp"

namespace ft {

/**
 * @brief Utility functions for generating controlled synthetic image changes.
 *
 * These functions are useful for demos and tests. They simulate movement
 * between two frames without introducing real image-processing complexity.
 */
class ImageTransform {
public:
    /**
     * @brief Translate all active pixels by (dx, dy).
     *
     * Pixels shifted outside the image bounds are discarded.
     *
     * @param image Input binary image.
     * @param dx Horizontal shift. Positive values move features right.
     * @param dy Vertical shift. Positive values move features down.
     * @return New translated binary image.
     */
    [[nodiscard]] static BinaryImage translate(
        const BinaryImage& image,
        int dx,
        int dy
    );
};

} // namespace ft
