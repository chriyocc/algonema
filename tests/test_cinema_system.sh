#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
SRC="$ROOT_DIR/StudentID1_StudentID2_StudentID3.c"

if [[ ! -f "$SRC" ]]; then
  echo "Missing source file: $SRC" >&2
  exit 1
fi

TMP_DIR="$(mktemp -d)"
trap 'rm -rf "$TMP_DIR"' EXIT

cp "$SRC" "$TMP_DIR/cinema_system.c"
(
  cd "$TMP_DIR"
  gcc -std=c99 -Wall -Wextra -o cinema_system cinema_system.c
)

run_case() {
  local name="$1"
  local input="$2"
  local output_file="$TMP_DIR/$name.out"

  (
    cd "$TMP_DIR"
    printf "%b" "$input" | ./cinema_system > "$output_file"
  )

  echo "$output_file"
}

assert_contains() {
  local file="$1"
  local expected="$2"

  if ! grep -Fq "$expected" "$file"; then
    echo "Expected to find '$expected' in $file" >&2
    echo "--- output ---" >&2
    cat "$file" >&2
    exit 1
  fi
}

assert_not_contains() {
  local file="$1"
  local unexpected="$2"

  if grep -Fq "$unexpected" "$file"; then
    echo "Did not expect to find '$unexpected' in $file" >&2
    echo "--- output ---" >&2
    cat "$file" >&2
    exit 1
  fi
}

view_output="$(run_case view_default '1\n1\n0\n')"
assert_contains "$view_output" "10 movie(s) loaded."
assert_contains "$view_output" "Interstellar"
assert_contains "$view_output" "The Dark Knight"
[[ "$(wc -l < "$TMP_DIR/movies.txt")" -eq 10 ]]

add_search_output="$(run_case add_search '1\n2\nDune Part Two\nSci-Fi\n166\n8.5\n2024\n5\n1\ndune\n0\n0\n')"
assert_contains "$add_search_output" "Movie added successfully."
assert_contains "$add_search_output" "Dune Part Two"
assert_contains "$add_search_output" "1 result(s) found."
assert_contains "$TMP_DIR/movies.txt" "11|Dune Part Two|Sci-Fi|166|8.5|2024"

edit_delete_output="$(run_case edit_delete '1\n3\n11\n4\n8.7\n4\n11\ny\n1\n0\n')"
assert_contains "$edit_delete_output" "Movie updated successfully."
assert_contains "$edit_delete_output" "Movie deleted successfully."
assert_not_contains "$TMP_DIR/movies.txt" "Dune Part Two"

validation_output="$(run_case validation '1\n2\nBad|Title\nValid Title\nAction\n0\n120\n11\n9.1\n1800\n2000\n0\n')"
assert_contains "$validation_output" "Input cannot contain the pipe character."
assert_contains "$validation_output" "Value must be greater than 0."
assert_contains "$validation_output" "Value must be between 0.0 and 10.0."
assert_contains "$validation_output" "Value must be between 1888 and 2100."
assert_contains "$validation_output" "Movie added successfully."

sort_output="$(run_case sort_no_save '1\n6\n2\nn\n0\n')"
assert_contains "$sort_output" "Sorted by rating (highest first)."
assert_contains "$sort_output" "Save this sorted order to file? (y/n):"

echo "All cinema system tests passed."
