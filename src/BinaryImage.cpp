#include "feature_tracking/BinaryImage.hpp"

#include <stdexcept>
#include <utility>

namespace ft {

BinaryImage::BinaryImage(Grid grid)
    : grid_(std::move(grid)) {
    validate(grid_);
}

std::size_t BinaryImage::width() const noexcept {
    return grid_.empty() ? 0U : grid_.front().size();
}

std::size_t BinaryImage::height() const noexcept {
    return grid_.size();
}

int BinaryImage::at(std::size_t x, std::size_t y) const {
    if (y >= height() || x >= width()) {
        throw std::out_of_range("BinaryImage::at: coordinate out of bounds");
    }

    return grid_[y][x];
}

const BinaryImage::Grid& BinaryImage::data() const noexcept {
    return grid_;
}

// Validates the binary image
void BinaryImage::validate(const Grid& grid) {
    if (grid.empty()) {
        throw std::invalid_argument("BinaryImage: grid must not be empty");
    }

    if (grid.front().empty()) {
        throw std::invalid_argument("BinaryImage: grid width must not be zero");
    }

    const auto expectedWidth = grid.front().size();

    for (const auto& row : grid) {
        if (row.size() != expectedWidth) {
            throw std::invalid_argument("BinaryImage: grid must be rectangular");
        }

        for (const int value : row) {
            if (value != 0 && value != 1) {
                throw std::invalid_argument(
                    "BinaryImage: grid values must be binary, i.e., 0 or 1"
                );
            }
        }
    }
}

} // namespace ft
