#!/usr/bin/env bash

# Default values
CONFIGURATION="debug"
APP_NAME="Grid-Test"

# Parse command-line arguments
while [[ "$#" -gt 0 ]]; do
    case $1 in
        --debug) CONFIGURATION="debug"; shift ;;  # Set configuration to debug
        --release) CONFIGURATION="release"; shift ;;  # Set configuration to release
        *) APP_NAME="$1"; shift ;;  # Set the application name (e.g., Grid-Test)
    esac
done

# Convert the configuration to match the folder names (Debug / Release)
CONFIGURATION_CAPITALIZED=$(echo "$CONFIGURATION" | awk '{print toupper(substr($0,1,1)) tolower(substr($0,2))}')

# Generate the Makefile using Premake
premake5 gmake2

# Run make with the specified configuration
make config="$CONFIGURATION"

# Ensure that the correct executable exists
EXEC_PATH="./bin/$CONFIGURATION_CAPITALIZED/$APP_NAME"

if [ ! -f "$EXEC_PATH" ]; then
    echo "Error: Executable $EXEC_PATH not found!"
    exit 1
fi

# Execute the app
exec "$EXEC_PATH"

