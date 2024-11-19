build:
	gcc -o Ray-of-Life main.c -lraylib -lm
test:
	gcc -o Test-of-Life example.c -lraylib -lm
