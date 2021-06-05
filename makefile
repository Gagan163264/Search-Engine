main.o:	main.c
	@gcc -c main.c

run: main.o
	@gcc main.o -o main.out
	@chmod 755 main.out
	@./main.out

compile: main.o

clean:
		@rm main.o
		@rm main.out
