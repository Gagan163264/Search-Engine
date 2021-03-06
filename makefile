main.o:	main.c ./func/databasehandling.c ./func/search.c
	@gcc -c main.c ./func/databasehandling.c ./func/search.c

run: main.o
	@gcc -g main.o databasehandling.o search.o -o main.out -lm
	@chmod 755 main.out
	@./main.out

compile: main.o

indexer.o:indexer.c ./func/databasehandling.c ./func/search.c
	@gcc -c indexer.c ./func/databasehandling.c ./func/search.c

crawl:indexer.o
	@gcc -g indexer.o databasehandling.o search.o -o crawl.out -lm
	@chmod 755 crawl.out
	@./crawl.out

ccompile: indexer.o

clean:
		@rm -f main.o
		@rm -f main.out
		@rm -f indexer.o
		@rm -f crawl.out
		@rm -f search.o
		@rm -f a.out
		@rm -f databasehandling.o
		@rm -f core
