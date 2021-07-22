main.o:	main.c databasehandling.c search.c
	@gcc -c main.c databasehandling.c search.c

run: main.o
	@gcc main.o databasehandling.o search.o -o main.out
	@chmod 755 main.out
	@./main.out

compile: main.o

indexer.o:indexer.c databasehandling.c
	@gcc -c indexer.c databasehandling.c

crawl:indexer.o
	@gcc indexer.o databasehandling.o -o crawl.out
	@chmod 755 crawl.out
	@./crawl.out

clean:
		@rm main.o
		@rm main.out
		@rm indexer.o
		@rm crawl.out
		@rm search.o
		@rm a.out
