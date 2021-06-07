#include<stdio.h>
#include<string.h>
#include<time.h>
#include"databasehandling.h"
#include"search.h"

#define INPUT_LIMIT 50


int main()
{
  int total_hit_counter = 0;
  clock_t timer;
  long database_size = 0;   //init
  char *database;
  database = importdb(&database_size);

  char uin_str[INPUT_LIMIT];                 // get user input
  printf("Search For: ");
  fgets(uin_str, INPUT_LIMIT, stdin);
  timer = clock();
  printf("Running search for: %s", uin_str);
  uin_str[strcspn(uin_str, "\n")] = 0;

  total_hit_counter = simple_search(uin_str, database, database_size);
  timer = clock() - timer;
  double time = ((double)timer)/(CLOCKS_PER_SEC/1000);

  printf("\nSimple search took %.3lf milliseconds to return %d results\n", time, total_hit_counter);
}
