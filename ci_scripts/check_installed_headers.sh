#!/bin/bash

# Check sm modules (Seb's maths library). Run this from build/ after ninja install
sm_modules=$(find ../sm -type f | grep -v "CMakeLists.txt" | rev | cut -d"/" -f1 | rev)
installed_modules=$(cat install_manifest.txt | rev | cut -d"/" -f1 | rev)
missing_modules=$(echo $sm_modules | tr ' ' '\n' | grep -Fxv -f <(echo $installed_modules | tr ' ' '\n'))

if [[ -z "$missing_modules" ]]; then
    echo "All modules from the sm directory have been installed successfully."
    exit 0
else
    echo "The following modules are in the sm directory, but were not installed:"
    echo "$missing_modules"
    exit 1
fi
