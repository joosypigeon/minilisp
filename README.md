# minilisp

You need to install GMP for large integer support.
sudo apt install libgmp-dev

Compile with minima debug
gcc -DDEBUG_LEVEL=DEBUG_LEVEL_ERROR -std=c99 -g -o minilisp *.c -lgmp

Compile with more debug than you can cope with
gcc -DDEBUG_LEVEL=DEBUG_LEVEL_VERBOSE -std=c99 -g -o minilisp *.c -lgmp
