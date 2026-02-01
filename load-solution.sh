#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <index.path> (e.g., 00.02.01)"
    exit 1
fi

IFS='.' read -r -a levels <<< "$1"

target_path="solutions/"

for level in "${levels[@]}"; do
    next_path=$(find "$target_path" -maxdepth 1 -type d -name "${level}" | tail -n 1)

    if [ -z "$next_path" ]; then
        echo "Error: Directory for index '$level' not found in $target_path"
        exit 1
    fi
    target_path="$next_path"
done

template_dir="template/"
if [ ! -d "$template_dir" ]; then
    echo "Error: Template '$template_dir' does not exist."
    exit 1
fi

echo "Cleaning workspace and loading: $target_path"

rm -rf workspace/
cp -r "$template_dir/" workspace/
rm -rf workspace/src/ workspace/tests/

if [ -d "$target_path/src/" ]; then
    cp -r "$target_path/src/" workspace/
fi

if [ -d "$target_path/tests/" ]; then
    cp -r "$target_path/tests/" workspace/
fi

echo "Successfully loaded $1 into the workspace."
