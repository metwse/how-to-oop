#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <index.path> (e.g., 00.02.01)"
    exit 1
fi

IFS='.' read -r -a levels <<< "$1"

target_path="."

for level in "${levels[@]}"; do
    next_path=$(find "$target_path" -maxdepth 1 -type d -name "${level}_*" | head -n 1)
    if [ -z "$next_path" ]; then
        echo "Error: Directory for index '$level' not found."
        exit 1
    fi
    target_path="$next_path"
done

echo "Saving current workspace to: $target_path"

if [ -d "workspace/src/" ]; then
    rm -rf "$target_path/src/"
    cp -r workspace/src/ "$target_path/"
fi

if [ -d "workspace/tests/" ]; then
    rm -rf "$target_path/tests/"
    cp -r workspace/tests/ "$target_path/"
fi

echo "Solution saved successfully."
