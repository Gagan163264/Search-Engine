main.o:	main.c databasehandling.c search.c
	@gcc -c main.c databasehandling.c search.c

run: main.o
	@gcc main.o databasehandling.o search.o -o main.out
	@chmod 755 main.out
	@./main.out

compile: main.o

clean:
		@rm main.o
		@rm main.out
