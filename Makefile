freeman: clean
	gcc -g activity.c balance.c main.c menu.c date.c input.c preferences.c project.c filesystem.c -o freeman -lcyaml

run: freeman
	./freeman

clean:
	-rm freeman
