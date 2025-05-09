#!/bin/bash

# Check sm headers (Seb's maths library)
sm_headers=$(find ../sm -type f | grep -v "fonts" | rev | cut -d"/" -f1 | rev | grep "\.h")
installed_headers=$(grep "\.h" install_manifest.txt | rev | cut -d"/" -f1 | rev)
missing_headers=$(echo $sm_headers | tr ' ' '\n' | grep -Fxv -f <(echo $installed_headers | tr ' ' '\n'))

if [[ -z "$missing_headers" ]]; then
    echo "All headers from the sm directory have been installed successfully."
    exit 0
else
    echo "The following headers are in the sm directory, but were not installed:"
    echo "$missing_headers"
    exit 1
fi
