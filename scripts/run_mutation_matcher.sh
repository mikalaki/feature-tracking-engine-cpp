#!/usr/bin/env bash
set -Eeuo pipefail

# Controlled matcher-focused mutation analysis.
# This script intentionally avoids heavyweight mutation frameworks so it can
# finish deterministically in CI and produce a compact reportable summary.

SOURCE_FILE="${SOURCE_FILE:-src/NearestNeighborMatcher.cpp}"
BUILD_DIR="${BUILD_DIR:-build-mutation}"
TEST_TIMEOUT_SECONDS="${TEST_TIMEOUT_SECONDS:-20}"
TEST_REGEX="${TEST_REGEX:-nearest_neighbor_matcher_tests}"

if [[ ! -f "$SOURCE_FILE" ]]; then
  echo "ERROR: source file not found: $SOURCE_FILE" >&2
  exit 2
fi

ORIGINAL_CONTENT="$(mktemp)"
cp "$SOURCE_FILE" "$ORIGINAL_CONTENT"

restore_source() {
  cp "$ORIGINAL_CONTENT" "$SOURCE_FILE"
}
trap restore_source EXIT

configure_and_build() {
  cmake -S . -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Debug >/dev/null
  cmake --build "$BUILD_DIR" --parallel 2 >/dev/null
}

run_tests() {
  timeout "${TEST_TIMEOUT_SECONDS}s" ctest --test-dir "$BUILD_DIR" --output-on-failure -R "$TEST_REGEX" >/tmp/mutation_test_output.txt 2>&1
}

apply_mutation() {
  local mutation_id="$1"

  case "$mutation_id" in
    M1_THRESHOLD_INCLUSIVE_TO_STRICT)
      perl -0pi -e 's/bestDistance <= options\.maxDistance/bestDistance < options.maxDistance/' "$SOURCE_FILE"
      ;;
    M2_DISABLE_UNMATCHED_INCREMENT)
      perl -0pi -e 's/\+\+result\.unmatchedA;/\/\/ MUTANT: unmatched increment removed/' "$SOURCE_FILE"
      ;;
    M3_DISABLE_UNIQUE_BOOKKEEPING)
      perl -0pi -e 's/usedB\[bestIndexB\] = true;/\/\/ MUTANT: unique-match bookkeeping removed/' "$SOURCE_FILE"
      ;;
    M4_INVERT_NEGATIVE_THRESHOLD_GUARD)
      perl -0pi -e 's/options\.maxDistance < 0\.0/options.maxDistance > 0.0/' "$SOURCE_FILE"
      ;;
    M5_DISTANCE_DY_IGNORED)
      perl -0pi -e 's/return std::sqrt\(dx \* dx \+ dy \* dy\);/return std::sqrt(dx * dx);/' "$SOURCE_FILE"
      ;;
    *)
      echo "ERROR: unknown mutation id: $mutation_id" >&2
      exit 3
      ;;
  esac
}

mutations=(
  M1_THRESHOLD_INCLUSIVE_TO_STRICT
  M2_DISABLE_UNMATCHED_INCREMENT
  M3_DISABLE_UNIQUE_BOOKKEEPING
  M4_INVERT_NEGATIVE_THRESHOLD_GUARD
  M5_DISTANCE_DY_IGNORED
)

echo "== Matcher-focused controlled mutation analysis =="
echo "Source file: $SOURCE_FILE"
echo "Build dir:   $BUILD_DIR"
echo "Test regex:  $TEST_REGEX"
echo "Timeout:     ${TEST_TIMEOUT_SECONDS}s per mutant"
echo

echo "[baseline] Building and running original tests..."
restore_source
configure_and_build
if ! run_tests; then
  echo "ERROR: baseline tests failed. Mutation analysis aborted."
  cat /tmp/mutation_test_output.txt
  exit 4
fi
echo "[baseline] PASS"
echo

killed=0
survived=0
timeout_count=0
build_failed=0

printf "%-38s | %-10s\n" "MUTANT" "RESULT"
printf "%-38s-+-%-10s\n" "--------------------------------------" "----------"

for mutation in "${mutations[@]}"; do
  restore_source
  apply_mutation "$mutation"

  if ! configure_and_build >/tmp/mutation_build_output.txt 2>&1; then
    printf "%-38s | %-10s\n" "$mutation" "INVALID"
    build_failed=$((build_failed + 1))
    continue
  fi

  set +e
  run_tests
  status=$?
  set -e

  if [[ $status -eq 0 ]]; then
    printf "%-38s | %-10s\n" "$mutation" "SURVIVED"
    survived=$((survived + 1))
  elif [[ $status -eq 124 ]]; then
    printf "%-38s | %-10s\n" "$mutation" "TIMEOUT"
    timeout_count=$((timeout_count + 1))
  else
    printf "%-38s | %-10s\n" "$mutation" "KILLED"
    killed=$((killed + 1))
  fi
done

restore_source

total_valid=$((killed + survived + timeout_count))
if [[ $total_valid -gt 0 ]]; then
  score=$((100 * killed / total_valid))
else
  score=0
fi

echo
echo "== Summary =="
echo "Killed mutants:   $killed"
echo "Survived mutants: $survived"
echo "Timeout mutants:  $timeout_count"
echo "Invalid mutants:  $build_failed"
echo "Mutation score:   ${score}%"
echo
echo "Mutation analysis completed."

# For CI: fail only if baseline was broken. Survived mutants are report data,
# not necessarily a pipeline failure for this coursework experiment.
exit 0
