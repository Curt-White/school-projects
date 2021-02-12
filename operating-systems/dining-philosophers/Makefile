FLAGS = $(-Wall -Werror -std=c11 -pedantic-errors -Wno-deprecated-declarations)
LIBS = $(-lpthread)

diningOut: diningOut.o
	gcc $(FLAGS) -o diningOut diningOut.o $(LIBS)

diningOut.o: diningOut.c
	gcc -c diningOut.c -o diningOut.o $(FLAGS) $(LIBS)

clean:
	rm diningOut diningOut.o