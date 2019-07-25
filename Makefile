CC=gcc
CFLAGS=-I.

client: client.c requests.c helpers.c tasks.c parson.c
		$(CC) -o client client.c requests.c helpers.c tasks.c parson.c -ggdb3 -Wall
		
run: client
		./client

clean:
		rm -f *.o client

check:
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=valgrind-out.txt ./client
