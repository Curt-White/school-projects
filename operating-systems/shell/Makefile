FLAGS = $(-Wall -Werror -std=c11 -pedantic-errors)

SEEsh: SEEsh.o
	gcc $(FLAGS) -o SEEsh SEEsh.o 

SEEsh.o: SEEsh.c
	gcc -c SEEsh.c -o SEEsh.o $(FLAGS)

clean:
	rm SEEsh SEEsh.o