#!/bin/bash

# Check sm headers (Seb's maths library). Run this from build/ after make install
sm_headers=$(find ../sm -type f | grep -v "CMakeLists.txt" | rev | cut -d"/" -f1 | rev)
installed_headers=$(cat install_manifest.txt | rev | cut -d"/" -f1 | rev)
missing_headers=$(echo $sm_headers | tr ' ' '\n' | grep -Fxv -f <(echo $installed_headers | tr ' ' '\n'))

if [[ -z "$missing_headers" ]]; then
    echo "All headers from the sm directory have been installed successfully."
    exit 0
else
    echo "The following headers are in the sm directory, but were not installed:"
    echo "$missing_headers"
    exit 1
fi
