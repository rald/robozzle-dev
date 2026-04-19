all: rle rld

rle: rle.c
	gcc rle.c -o rle

rld: rld.c
	gcc rld.c -o rld

.PHONY: enc dec clean

enc:
	./rle L00.TXT E00.TXT
	./rle L01.TXT E01.TXT
	./rle L02.TXT E02.TXT
	./rle L03.TXT E03.TXT
	cat E??.TXT > LEVELS.TXT

dec:
	./rld E00.TXT D00.TXT
	./rld E01.TXT D01.TXT
	./rld E02.TXT D02.TXT
	./rld E03.TXT D03.TXT

clean:
	rm E??.TXT D??.TXT rld rle

