# 🚀 Feature Tracking Engine C++

A lightweight C++ feature tracking engine used as a controlled **System Under Test (SUT)** for experimenting with automated testing, regression testing, mutation analysis, and continuous integration workflows.

The project models a simplified feature tracking pipeline inspired by classical computer vision systems while intentionally using deterministic synthetic binary images instead of real image files or OpenCV.

---

# 📚 Contents

- [🚀 Feature Tracking Engine C++](#-feature-tracking-engine-c)
- [📚 Contents](#-contents)
- [🔍 Overview](#-overview)
- [🏗️ Architecture](#️-architecture)
- [🧠 Core Concepts](#-core-concepts)
  - [Detector](#detector)
  - [Matcher](#matcher)
- [📦 Project Structure](#-project-structure)
- [⚙️ Build](#️-build)
- [▶️ Run Demo](#️-run-demo)
- [🧪 Testing Strategy](#-testing-strategy)
  - [GoogleTest and CTest](#googletest-and-ctest)
  - [Fast Suite](#fast-suite)
  - [Full Suite](#full-suite)
  - [Regression Suite](#regression-suite)
  - [Execute Individual CTest Tests](#execute-individual-ctest-tests)
  - [Execute Individual Regression Test Cases](#execute-individual-regression-test-cases)
- [🔄 CI Workflow Strategy](#-ci-workflow-strategy)
  - [On Push](#on-push)
  - [On Pull Request](#on-pull-request)
  - [Manual Mutation Analysis](#manual-mutation-analysis)
- [🧬 Mutation Testing](#-mutation-testing)
  - [Implemented Mutation Scenarios](#implemented-mutation-scenarios)
- [🧩 Implemented Modules](#-implemented-modules)
- [🚧 Future Improvements](#-future-improvements)
- [📄 License](#-license)

---

# 🔍 Overview

The system models the following simplified feature tracking pipeline:

```text
Binary Grid/Image A -> Detector -> Frame A
Binary Grid/Image B -> Detector -> Frame B
Frame A + Frame B -> Matcher -> Matches
```

The input is a deterministic synthetic binary image:

- `0` = background pixel
- `1` = feature pixel

This design keeps the system small, reproducible, and testable while still representing a realistic software component from a feature tracking pipeline.

The project focuses on:

- feature detection from binary grids
- feature matching between frames
- unit testing
- regression testing
- continuous integration testing
- CI-based feedback
- matcher-focused mutation analysis

---

# 🏗️ Architecture

```text
Binary Image/Grid
        ↓
   GridDetector
        ↓
 Frame = vector<Feature>
        ↓
NearestNeighborMatcher
        ↓
      Matches
```

The detector extracts feature points from a binary grid.  
The matcher compares features between two frames and produces correspondences.

---

# 🧠 Core Concepts

## Detector

The detector converts active pixels into feature points.

Detection rule:

```text
if pixel == 1 -> Feature(x, y)
```

Example input:

```text
0 1 0
0 0 1
1 0 0
```

Detected features:

```text
(1,0), (2,1), (0,2)
```

This intentionally simple detector provides deterministic behavior, which is useful for controlled testing and regression experiments.

---

## Matcher

The matcher performs brute-force nearest-neighbor matching.

For every feature in Frame A:

1. Compare against all features in Frame B
2. Compute Euclidean distance
3. Select the nearest feature
4. Accept the match if the distance is within the configured threshold

Distance formula:

```text
d = sqrt((x1 - x2)^2 + (y1 - y2)^2)
```

Matcher characteristics:

- brute-force matching: `O(N × M)`
- deterministic output
- configurable distance threshold
- optional unique-match enforcement
- suitable for regression and mutation testing

---

# 📦 Project Structure

```text
feature-tracking-engine-cpp/
├── app/                    # Demo application
├── include/                # Public headers
├── scripts/                # Automation and mutation scripts
├── src/                    # Core implementation
├── tests/                  # GoogleTest and regression tests
├── .github/workflows/      # GitHub Actions CI workflows
├── CMakeLists.txt
└── README.md
```

---

# ⚙️ Build

From the repository root:

```bash
mkdir -p build
cmake -S . -B build
cmake --build build
```

---

# ▶️ Run Demo

```bash
./build/feature_tracking_demo
```

The demo:

- creates a synthetic binary image
- generates a translated/moving image
- detects features in both images
- matches features between frames
- prints the resulting correspondences

---

# 🧪 Testing Strategy

The project separates tests into different execution levels to support a lean testing workflow.

| Test Level | Purpose | Typical Use |
|---|---|---|
| Fast suite | Quick feedback |  every push |
| Full suite | Complete verification | pull request / before merge |
| Regression suite | End-to-end behavior verification | pull request / full suite |
| Mutation analysis | Test quality evaluation | manual / extended workflow |

This separation avoids running expensive checks unnecessarily while still providing stronger verification before integration.

---

## GoogleTest and CTest

The project uses **GoogleTest** for implementing test cases and **CTest** for orchestrating test execution.

- GoogleTest defines assertions and individual test cases.
- CTest registers and executes test executables.
- CTest labels are used to separate fast, full, regression, unit, smoke, and mutation-related checks.
- GitHub Actions uses CTest commands to run the appropriate test level.

List all registered CTest tests:

```bash
ctest --test-dir build -N
```

---

## Fast Suite

The fast suite is intended for quick feedback during normal development.

It should be executed:

- locally before pushing
- automatically on every push
- as the first CI feedback layer

Run the fast suite **after building the project**:

```bash
ctest --test-dir build -L fast --output-on-failure
```
Run with verbose output:

```bash
ctest --test-dir build -L fast --output-on-failure -V
```
The fast suite currently includes **5 CTest tests**:

```text
binary_image_tests
grid_detector_tests
image_transform_tests
nearest_neighbor_matcher_tests
demo_smoke
```

---

## Full Suite

The full suite is intended for complete verification.

It should be executed:

- locally before important commits
- automatically on pull requests
- before merging changes

Run the full suite **after building the project**:

```bash
ctest --test-dir build -L full --output-on-failure
```

Run with verbose output:

```bash
ctest --test-dir build -L full --output-on-failure -V
```

The full suite currently includes **6 CTest tests**:

```text
binary_image_tests
grid_detector_tests
image_transform_tests
nearest_neighbor_matcher_tests
pipeline_regression_tests
demo_smoke
```

---

## Regression Suite

The regression suite validates stable behavior of the feature tracking pipeline across controlled changes.

The regression tests focus on:

- translated binary images
- stable feature correspondences
- clipped translations
- large synthetic image scenarios
- repeated motion patterns

Run the regression suite through CTest:

```bash
ctest --test-dir build -L regression --output-on-failure
```

Or run the regression executable directly:

```bash
./build/tests/pipeline_regression_tests
```

List all regression test cases:

```bash
./build/tests/pipeline_regression_tests --gtest_list_tests
```

---

## Execute Individual CTest Tests

List all registered CTest tests:

```bash
ctest --test-dir build -N
```

Run individual CTest executables:
e.g.: 
```bash
ctest --test-dir build -R binary_image_tests --output-on-failure
```

```bash
ctest --test-dir build -R grid_detector_tests --output-on-failure
```


## Execute Individual Regression Test Cases

List available GoogleTest regression cases:

```bash
./build/tests/pipeline_regression_tests --gtest_list_tests
```

Run each regression case individually:

```bash
./build/tests/pipeline_regression_tests --gtest_filter=PipelineRegression.TranslatedRegularPatternProducesStableCorrespondences
```

```bash
./build/tests/pipeline_regression_tests --gtest_filter=PipelineRegression.MultipleControlledTranslationsPreserveFeatureCountAndDistance
```

```bash
./build/tests/pipeline_regression_tests --gtest_filter=PipelineRegression.ClippedTranslationReducesMatchesWithoutCreatingFalseCorrespondences
```

```bash
./build/tests/pipeline_regression_tests --gtest_filter=PipelineRegression.LargerSyntheticImageStressScenarioPreservesAllMatches
```

```bash
./build/tests/pipeline_regression_tests --gtest_filter=PipelineRegression.LargeSyntheticImageRepeatedMotionStressScenario
```

These commands are useful when debugging one specific regression scenario without executing the entire regression executable.

---

# 🔄 CI Workflow Strategy

The CI setup follows a staged feedback strategy.

The goal is to run cheaper checks early and reserve heavier checks for integration or manual execution.

---

## On Push

Every push executes the **fast suite**.

Typical CI behavior:

```text
push
  -> configure project
  -> build project
  -> run fast suite
```

Command used:

```bash
ctest --test-dir build -L fast --output-on-failure
```

Expected scope:

```text
binary_image_tests
grid_detector_tests
image_transform_tests
nearest_neighbor_matcher_tests
demo_smoke
```

Purpose:

- provide fast developer feedback
- catch compilation errors
- catch core behavior failures
- avoid running heavier regression or mutation analysis on every small update

---

## On Pull Request

Pull requests execute the **full suite**, including regression tests.

Typical CI behavior:

```text
pull_request
  -> configure project
  -> build project
  -> run full suite
```

Command used:

```bash
ctest --test-dir build -L full --output-on-failure
```

Expected scope:

```text
binary_image_tests
grid_detector_tests
image_transform_tests
nearest_neighbor_matcher_tests
pipeline_regression_tests
demo_smoke
```

Purpose:

- provide stronger verification before merging
- protect the main branch from regressions
- validate important feature tracking behavior
- support continuous integration quality gates

---

## Manual Mutation Analysis

Mutation analysis is more expensive than the normal test suites, so it is intended to be executed manually or as an extended CI job.

Typical CI behavior:

```text
workflow_dispatch
  -> configure project
  -> run controlled matcher-focused mutation analysis
  -> report killed/survived/timeout/invalid mutants
```

Command used:

```bash
./scripts/run_mutation_matcher.sh
```

Purpose:

- evaluate the quality of matcher-focused tests
- identify weak or missing assertions
- avoid slowing down every push or pull request
- keep expensive analysis controlled and reproducible

---

# 🧬 Mutation Testing

The project includes controlled matcher-focused mutation analysis through:

```bash
./scripts/run_mutation_matcher.sh
```

Before running:

```bash
chmod +x scripts/run_mutation_matcher.sh
```

Run locally:

```bash
./scripts/run_mutation_matcher.sh
```

The mutation workflow:

1. Applies predefined source-level mutations to the matcher implementation
2. Rebuilds the project
3. Executes matcher-focused tests
4. Reports the mutation outcome

Reported categories:

| Category | Meaning |
|---|---|
| Killed mutant | Tests failed, so the injected fault was detected |
| Survived mutant | Tests passed, so the injected fault was not detected |
| Timeout mutant | Execution exceeded the allowed time |
| Invalid mutant | Mutated code did not compile or could not be evaluated |

Example output:

```text
Killed mutants:   4
Survived mutants: 1
Timeout mutants:  0
Invalid mutants:  0
Mutation score:   80%
```

---

## Implemented Mutation Scenarios

The mutation script applies controlled mutations to the matcher subsystem, such as:

- threshold condition modification
- distance comparison inversion
- nearest-neighbor selection corruption
- disabled distance updates
- altered match acceptance logic

These mutations are intentionally focused on the matcher because it is the main decision-making component of the SUT.

This keeps mutation analysis:

- bounded
- reproducible
- suitable for manual/extended CI execution
- useful for evaluating test effectiveness

---

# 🧩 Implemented Modules

| Module | Description |
|---|---|
| `BinaryImage` | Validated synthetic binary image/grid representation |
| `Feature` | 2D feature point |
| `Frame` | Collection of detected features |
| `GridDetector` | Extracts active pixels as features |
| `NearestNeighborMatcher` | Brute-force matcher with threshold filtering |
| `ImageTransform` | Creates controlled translated/moving images |
| `MatchResult` | Stores matching results and statistics |

---

# 🚧 Future Improvements

Possible future extensions include:

- descriptor-based matching
- feature prioritization strategies
- noise injection
- visualization of matches
- optimized matching algorithms
- scheduled mutation testing

---

# 📄 License

MIT License