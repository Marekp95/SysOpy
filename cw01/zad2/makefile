compile: compile_static compile_shared compile_dynamic
compile_static: main.c mylib.a 
	gcc -Wall -o main_static $^

compile_shared: mylib.so main.c
	gcc -o main_shared $^ -Wl,-rpath,./

compile_dynamic: main_dynamic.c
	gcc -o main_dynamic $^ -Wl,-rpath,./ -ldl

lib: mylib.a mylib.so
mylib.so: shared.o
	gcc -shared -fPIC -o $@ $^
	rm $^
shared.o: mylib.c
	gcc -c  -o $@ -fPIC $^

mylib.a: static.o
	ar cr $@ $^
	rm $^
static.o: mylib.c
	gcc -c -o $@ $^

clean:
	rm -f *~
	rm -f *.o
	rm -f *.a
	rm -f *.so
clean_all: clean
	rm -f main_dynamic
	rm -f main_shared
	rm -f main_static

test: compile
	./main_static
	./main_dynamic
	./main_shared
	make clean_all

