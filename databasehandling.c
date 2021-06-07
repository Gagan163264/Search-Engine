#include<stdio.h>
#include <stdlib.h>
#include<string.h>

#define DATABASE "db2.txt"



char* importdb(long *database_size)
{
  FILE *dbfile = fopen(DATABASE, "r");
  fseek(dbfile, 0L, SEEK_END);
  long fsize = ftell(dbfile);
  fseek(dbfile, 0, SEEK_SET);
  char *database = malloc(fsize + 1);
  fread(database, 1, fsize, dbfile);
  fclose(dbfile);
  database[fsize] = 0;
  *database_size = fsize;
  return database;
}
