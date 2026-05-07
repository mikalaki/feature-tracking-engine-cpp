#pragma once

#include <cstddef>
#include <vector>

namespace ft {

/**
 * @brief Represents a synthetic binary image/grid.
 *
 * Pixel value 0 means background.
 * Pixel value 1 means feature candidate.
 *
 * The class validates that:
 * - the grid is not empty,
 * - all rows have equal width,
 * - all values are binary, i.e., 0 or 1.
 */
class BinaryImage {
public:
    using Grid = std::vector<std::vector<int>>;

    /**
     * @brief Construct a binary image from a 2D grid.
     * @throws std::invalid_argument if the grid is empty, non-rectangular,
     * or contains values other than 0 and 1.
     */
    explicit BinaryImage(Grid grid);

    /**
     * @brief Return image width in pixels.
     */
    [[nodiscard]] std::size_t width() const noexcept;

    /**
     * @brief Return image height in pixels.
     */
    [[nodiscard]] std::size_t height() const noexcept;

    /**
     * @brief Return pixel value at coordinate (x, y).
     * @throws std::out_of_range if x or y is outside image bounds.
     */
    [[nodiscard]] int at(std::size_t x, std::size_t y) const;

    /**
     * @brief Return the underlying grid.
     */
    [[nodiscard]] const Grid& data() const noexcept;

private:
    Grid grid_;

    static void validate(const Grid& grid);
};

} // namespace ft
