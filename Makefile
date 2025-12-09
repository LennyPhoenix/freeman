freeman: clean
	gcc -g main.c menu.c input.c -o freeman

run: freeman
	./freeman

clean:
	-rm freeman
