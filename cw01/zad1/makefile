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
	rm mylib.a
	rm -f mylib.so