#!/bin/bash

# test_harness.sh

LISP="./minilisp"
PASS=0
FAIL=0

for file in tests/*.lisp; do
    expected_file="${file%.lisp}.out"
    
    if [ ! -f "$expected_file" ]; then
        echo "SKIP: No expected output for $file"
        continue
    fi

    actual_output=$(mktemp)
    "$LISP" "$file" > "$actual_output"

    if diff -q "$actual_output" "$expected_file" > /dev/null; then
        echo "PASS: $file"
        PASS=$((PASS + 1))
    else
        echo "FAIL: $file"
        echo "Expected:"
        cat "$expected_file"
        echo "Got:"
        cat "$actual_output"
        FAIL=$((FAIL + 1))
    fi

    rm "$actual_output"
done

echo
echo "Total: $((PASS + FAIL)), Passed: $PASS, Failed: $FAIL"
