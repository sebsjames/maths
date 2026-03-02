mkdir build
cd build
cmake .. -GNinja
# make all verbose
cmake --build . --target all --verbose

#
# Other options
#

# to clean:
cmake --build . --target clean

# One target non-verbose:
cmake --build . --target use_range use_range2

# Two targets, verbose:
cmake --build . --target use_range use_range2 --verbose
