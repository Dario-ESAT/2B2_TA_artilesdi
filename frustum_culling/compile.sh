
g++ -c -Wall -O2 -Igml/ -o buildobjs/frustum_culling.o frustum_culling.cpp

cd buildobjs
gcc -o ../frustum_culling.elf frustum_culling.o -lSDL -lm
