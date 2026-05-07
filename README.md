# Feature Tracking Engine C++

A lightweight C++ system under test (SUT) for experimenting with automated testing and continuous integration.

The project models a simplified feature tracking pipeline:

```text
Binary Grid/Image A -> Detector -> Frame A
Binary Grid/Image B -> Detector -> Frame B
Frame A + Frame B -> Matcher -> Matches
```

## Scope

This project intentionally uses synthetic binary images instead of real image files or OpenCV.

- `0` = background pixel
- `1` = feature pixel

The detector converts active pixels into feature points. The matcher then compares detected features between two frames and produces correspondences.

## Build

```bash
mkdir -p build
cd build
cmake ..
cmake --build .
./feature_tracking_demo
```

## Modules

- `BinaryImage`: validated 2D binary grid representation
- `GridDetector`: converts active pixels into `Feature(x, y)` points
- `Frame`: vector of detected features
- `NearestNeighborMatcher`: brute-force nearest-neighbor matcher with thresholding
- `ImageTransform`: helper for controlled synthetic motion examples
