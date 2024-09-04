all: snake

snake: snake.c
	gcc -o snake snake.c ./raylib-5.0_linux_amd64/lib/libraylib.a -I./raylib-5.0_linux_amd64/include -lm -ggdb