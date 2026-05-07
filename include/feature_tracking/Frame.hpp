#pragma once

#include "feature_tracking/Feature.hpp"

#include <vector>

namespace ft {

/**
 * @brief A frame is a collection of detected features.
 *
 * Conceptually, this represents the output of the Detector module for one
 * binary image/grid.
 */
using Frame = std::vector<Feature>;

} // namespace ft
