#!/bin/bash

# Get the current directory
SCRIPT_FOLDER="$(pwd)"

# Loop through all .sh files in the current directory
for script in "$SCRIPT_FOLDER"/*.bash; do
    # Skip this script itself
    if [ "$script" == "$0" ]; then
        continue
    fi

    # Check if there are any .sh files
    if [ ! -e "$script" ]; then
        echo "No .sh scripts found in $SCRIPT_FOLDER."
        exit 1
    fi

    # Make the script executable if it's not already
    chmod +x "$script"

    # Run the script
    echo "Running: $script"
    "$script"

    # Check the exit status
    if [ $? -ne 0 ]; then
        echo "Error: Script $script failed."
        exit 1
    fi
done

echo "All scripts executed successfully."
