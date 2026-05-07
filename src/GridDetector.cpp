#include "feature_tracking/GridDetector.hpp"

namespace ft {

Frame GridDetector::detect(const BinaryImage& image) const {
    Frame features;

    for (std::size_t y = 0; y < image.height(); ++y) {
        for (std::size_t x = 0; x < image.width(); ++x) {
            if (image.at(x, y) == 1) {
                features.push_back(
                    Feature{
                        static_cast<int>(x),
                        static_cast<int>(y)
                    }
                );
            }
        }
    }

    return features;
}

} // namespace ft
