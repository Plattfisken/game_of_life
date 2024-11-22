mkdir -p ../build;
Clang main.c -L ../lib/ -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL ../lib/libraylib.a -o ../build/game_of_life -g -O2 -Wall;
