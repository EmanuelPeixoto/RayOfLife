build:
	gcc -o Ray-of-Life main.c -lraylib
test:
	gcc -o Test-of-Life example.c -lraylib
