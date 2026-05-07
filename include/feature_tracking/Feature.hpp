#pragma once

#include <ostream>

namespace ft {

/**
 * @brief Represents a detected feature point in a 2D image/grid.
 *
 * In this simplified SUT, a feature corresponds to the coordinate of a pixel
 * with value 1 in a binary image.
 */
struct Feature {
    int x{};
    int y{};

    bool operator==(const Feature& other) const noexcept {
        return x == other.x && y == other.y;
    }
};

/**
 * @brief Pretty-print helper for demo/debug output.
 */
inline std::ostream& operator<<(std::ostream& os, const Feature& feature) {
    os << "(" << feature.x << ", " << feature.y << ")";
    return os;
}

} // namespace ft
