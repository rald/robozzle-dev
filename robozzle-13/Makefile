all: rle rld game

rle: rle.c
	gcc rle.c -o rle

rld: rld.c
	gcc rld.c -o rld

game: main.c
	gcc main.c -o game -lncurses -g

.PHONY: enc dec clean

enc:
	./rle LEVELS/L00.TXT LEVELS/E00.TXT
	./rle LEVELS/L01.TXT LEVELS/E01.TXT
	./rle LEVELS/L02.TXT LEVELS/E02.TXT
	./rle LEVELS/L03.TXT LEVELS/E03.TXT
	cat LEVELS/E??.TXT > LEVELS.TXT

dec:
	./rld LEVELS/E00.TXT LEVELS/D00.TXT
	./rld LEVELS/E01.TXT LEVELS/D01.TXT
	./rld LEVELS/E02.TXT LEVELS/D02.TXT
	./rld LEVELS/E03.TXT LEVELS/D03.TXT

clean:
	rm LEVELS/E??.TXT LEVELS/D??.TXT rld rle game


