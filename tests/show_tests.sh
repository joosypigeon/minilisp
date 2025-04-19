#!/bin/bash

# Directory to search
DIR="."

# Find and sort all .c and .h files in the directory
FILES=$(find "$DIR" -maxdepth 1 -type f \( -name "*.lisp" -o -name "*.out" \) | sort)

# Loop through each file
for FILE in $FILES; do
    echo "===== $FILE ====="
    cat "$FILE"
    echo
done
