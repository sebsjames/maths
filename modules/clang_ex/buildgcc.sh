# What's the g++ equivalent to build the clange example?
g++ -std=c++20 Hello.cppm --precompile_equivalent? -o Hello.pcm
g++ -std=c++20 use.cpp -fmodule-file=Hello=Hello.pcm Hello.pcm -o Hello.out
