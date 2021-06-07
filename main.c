#include<stdio.h>
#include<string.h>
#include"databasehandling.h"
#include"search.h"

#define INPUT_LIMIT 50


int main()
{
  long database_size = 0;
  char *database;
  database = importdb(&database_size);
  //printf("file :%s", database);

  char uin_str[INPUT_LIMIT];                 // get user input
  printf("Search For: ");
  fgets(uin_str, INPUT_LIMIT, stdin);
  printf("Running search for: %s", uin_str);
  uin_str[strcspn(uin_str, "\n")] = 0;

  simple_search(uin_str, database, database_size);
 }
