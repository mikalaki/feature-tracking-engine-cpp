#include "feature_tracking/ImageTransform.hpp"

namespace ft {

BinaryImage ImageTransform::translate(
    const BinaryImage& image,
    int dx,
    int dy
) {
    BinaryImage::Grid translated(
        image.height(),
        std::vector<int>(image.width(), 0)
    );

    for (std::size_t y = 0; y < image.height(); ++y) {
        for (std::size_t x = 0; x < image.width(); ++x) {
            if (image.at(x, y) != 1) {
                continue;
            }

            const int newX = static_cast<int>(x) + dx;
            const int newY = static_cast<int>(y) + dy;

            if (
                newX >= 0 &&
                newY >= 0 &&
                newX < static_cast<int>(image.width()) &&
                newY < static_cast<int>(image.height())
            ) {
                translated[static_cast<std::size_t>(newY)]
                          [static_cast<std::size_t>(newX)] = 1;
            }
        }
    }

    return BinaryImage{translated};
}

} // namespace ft
