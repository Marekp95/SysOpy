ELEM = 1000
ELEM2 = 2000
compile_main:
	gcc main.c -o main -std=c99 -Wall
compile_gen:
	gcc gen.c -o gen -std=c99 -Wall

test: compile_main compile_gen
	./gen d.txt 4 $(ELEM)
	./main d.txt 4 lib
	./gen d.txt 4 $(ELEM)
	./main d.txt 4 sys

	./gen d.txt 512 $(ELEM)
	./main d.txt 512 lib
	./gen d.txt 512 $(ELEM)
	./main d.txt 512 sys

	./gen d.txt 1024 $(ELEM)
	./main d.txt 1024 lib
	./gen d.txt 1024 $(ELEM)
	./main d.txt 1024 sys

	./gen d.txt 4096 $(ELEM)
	./main d.txt 4096 lib
	./gen d.txt 4096 $(ELEM)
	./main d.txt 4096 sys

	./gen d.txt 8192 $(ELEM)
	./main d.txt 8192 lib
	./gen d.txt 8192 $(ELEM)
	./main d.txt 8192 sys

	rm -f d.txt

	./gen d.txt 4 $(ELEM2)
	./main d.txt 4 lib
	./gen d.txt 4 $(ELEM2)
	./main d.txt 4 sys

	./gen d.txt 512 $(ELEM2)
	./main d.txt 512 lib
	./gen d.txt 512 $(ELEM2)
	./main d.txt 512 sys

	./gen d.txt 1024 $(ELEM2)
	./main d.txt 1024 lib
	./gen d.txt 1024 $(ELEM2)
	./main d.txt 1024 sys

	./gen d.txt 4096 $(ELEM2)
	./main d.txt 4096 lib
	./gen d.txt 4096 $(ELEM2)
	./main d.txt 4096 sys

	./gen d.txt 8192 $(ELEM2)
	./main d.txt 8192 lib
	./gen d.txt 8192 $(ELEM2)
	./main d.txt 8192 sys

	rm -f d.txt
	make clean
clean:
	rm -f gen
	rm -f main