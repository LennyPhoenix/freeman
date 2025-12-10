freeman: clean
	gcc -g main.c menu.c input.c preferences.c filesystem.c -o freeman -lcyaml

run: freeman
	./freeman

clean:
	-rm freeman
