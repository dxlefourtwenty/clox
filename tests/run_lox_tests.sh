#!/usr/bin/env bash

set -u

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
TEST_DIR="${1:-$ROOT_DIR/tests/ternary}"

if [[ ! -d "$TEST_DIR" ]]; then
  echo "missing test directory: $TEST_DIR" >&2
  exit 1
fi

failures=0
total=0

run_case() {
  local file="$1"
  local expected_status
  local expected_substring
  local output_file
  local status

  expected_status="$(sed -n 's#// expect-status: ##p' "$file" | head -n 1)"
  expected_substring="$(sed -n 's#// expect-substring: ##p' "$file" | head -n 1)"

  if [[ -z "$expected_status" ]]; then
    echo "missing // expect-status directive in $file" >&2
    failures=$((failures + 1))
    return
  fi

  output_file="$(mktemp)"
  if env ASAN_OPTIONS=detect_leaks=0 LSAN_OPTIONS=detect_leaks=0 \
      "$ROOT_DIR/main" "$file" >"$output_file" 2>&1; then
    status=0
  else
    status=$?
  fi

  total=$((total + 1))

  if [[ "$expected_status" == "ok" && "$status" -ne 0 ]]; then
    echo "FAIL $file" >&2
    cat "$output_file" >&2
    failures=$((failures + 1))
    rm -f "$output_file"
    return
  fi

  if [[ "$expected_status" == "compile_error" && "$status" -ne 65 ]]; then
    echo "FAIL $file" >&2
    echo "expected compile error exit 65, got $status" >&2
    cat "$output_file" >&2
    failures=$((failures + 1))
    rm -f "$output_file"
    return
  fi

  if [[ -n "$expected_substring" ]] &&
      ! grep -Fq "$expected_substring" "$output_file"; then
    echo "FAIL $file" >&2
    echo "missing expected output substring: $expected_substring" >&2
    cat "$output_file" >&2
    failures=$((failures + 1))
    rm -f "$output_file"
    return
  fi

  echo "PASS $file"
  rm -f "$output_file"
}

while IFS= read -r file; do
  run_case "$file"
done < <(find "$TEST_DIR" -type f -name '*.lox' | sort)

echo "$total tests run"

if [[ "$failures" -ne 0 ]]; then
  echo "$failures test(s) failed" >&2
  exit 1
fi
