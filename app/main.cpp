#include "feature_tracking/BinaryImage.hpp"
#include "feature_tracking/GridDetector.hpp"
#include "feature_tracking/ImageTransform.hpp"
#include "feature_tracking/NearestNeighborMatcher.hpp"

#include <exception>
#include <iomanip>
#include <iostream>
#include <string>

namespace {

/**
 * @brief Print a frame of detected features.
 */
void printFrame(const std::string& name, const ft::Frame& frame) {
    std::cout << name << " contains " << frame.size() << " features:\n";

    for (std::size_t i = 0; i < frame.size(); ++i) {
        std::cout << "  [" << i << "] " << frame[i] << '\n';
    }
}

/**
 * @brief Print a binary image grid.
 */
void printImage(const std::string& name, const ft::BinaryImage& image) {
    std::cout
        << name << " (" << image.width() << "x" << image.height() << "):\n";

    for (const auto& row : image.data()) {
        std::cout << "  ";
        for (std::size_t x = 0; x < row.size(); ++x) {
            std::cout << row[x];
            if (x + 1 < row.size()) {
                std::cout << ' ';
            }
        }
        std::cout << '\n';
    }
}

/**
 * @brief Print accepted matches.
 */
void printMatches(
    const ft::Frame& frameA,
    const ft::Frame& frameB,
    const ft::MatchResult& result
) {
    std::cout << "\nAccepted matches:\n";

    if (result.matches.empty()) {
        std::cout << "  No matches found.\n";
        return;
    }

    for (const auto& match : result.matches) {
        std::cout
            << "  A[" << match.indexA << "] " << frameA[match.indexA]
            << " -> "
            << "B[" << match.indexB << "] " << frameB[match.indexB]
            << " | distance = "
            << std::fixed << std::setprecision(2)
            << match.distance
            << '\n';
    }

    std::cout
        << "\nSummary:\n"
        << "  Matches: " << result.matches.size() << '\n'
        << "  Unmatched features in A: " << result.unmatchedA << '\n'
        << "  Average match distance: "
        << std::fixed << std::setprecision(2)
        << result.averageDistance() << '\n';
}

} // namespace

int main() {
    try {
        /*
         * Example:
         *
         * The following binary grid is treated as a synthetic image:
         *   0 = background pixel
         *   1 = feature pixel
         *
         * The detector extracts each 1-valued pixel as Feature(x, y).
         */
        const ft::BinaryImage imageA{
            {
                {0, 1, 0, 0, 0},
                {0, 0, 0, 1, 0},
                {1, 0, 0, 0, 0},
                {0, 0, 1, 0, 0},
                {0, 0, 0, 0, 1}
            }
        };

        /*
         * Simulate a moving image/frame.
         *
         * This creates imageB by translating active pixels from imageA by:
         *   dx = +1  -> move one cell to the right
         *   dy =  0  -> no vertical movement
         *
         * This is a simple way to model motion between consecutive frames.
         */
        const ft::BinaryImage imageB = ft::ImageTransform::translate(
            imageA,
            1,
            0
        );

        const ft::GridDetector detector;

        const ft::Frame frameA = detector.detect(imageA);
        const ft::Frame frameB = detector.detect(imageB);

        printImage("Image A", imageA);
        std::cout << '\n';
        printImage("Image B", imageB);
        std::cout << "\n";

        printFrame("Frame A", frameA);
        std::cout << '\n';
        printFrame("Frame B", frameB);

        /*
         * The matcher compares each feature in Frame A against all features
         * in Frame B, selects the nearest one, and accepts it only if it is
         * within maxDistance.
         *
         * With maxDistance = 1.1, a one-cell horizontal movement is accepted.
         */
        const ft::NearestNeighborMatcher matcher;

        const ft::MatchOptions options{
            .maxDistance = 1.1,
            .enforceUniqueMatches = true
        };

        const ft::MatchResult result = matcher.match(
            frameA,
            frameB,
            options
        );

        printMatches(frameA, frameB, result);

        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "Application error: " << ex.what() << '\n';
        return 1;
    }
}
