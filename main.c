#include<stdio.h>
#include<string.h>
#include<time.h>
#include<ctype.h>

#include"databasehandling.h"
#include"search.h"

#define INPUT_LIMIT 50


int main()
{
  int total_hit_counter = 0;
  int start_index = 0;
  int end_index;
  int write = 0;
  int multiplewordin_flag = 0;
  clock_t timer;
  long database_size = 0;   //init
  char *database;
  database = importdb(&database_size);

  char uin_str[INPUT_LIMIT];                 // get user input
  char raw_uin_str[INPUT_LIMIT];
  printf("Search For: ");
  fgets(raw_uin_str, INPUT_LIMIT, stdin);
  timer = clock();

  while(raw_uin_str[start_index] == ' ')    //trim raw input
    start_index++;
  end_index = strcspn(raw_uin_str, "\n")-1;
  while(raw_uin_str[end_index] == ' ')
    end_index--;
  while(start_index<=end_index)
  {
    uin_str[write++]=raw_uin_str[start_index];
    start_index++;
  }
  uin_str[start_index]=0;
  start_index = 0;
  while(start_index<write)
  {
    if(uin_str[start_index] == ' ')
      {
        multiplewordin_flag = 1;
        break;
      }
      start_index++;
  }

  printf("Running search for: %s\n", uin_str);
  total_hit_counter = simple_search(uin_str, database, database_size, multiplewordin_flag);
  timer = clock() - timer;
  double time = ((double)timer)/(CLOCKS_PER_SEC/1000000);
  printf("\nSimple search took %.1lf microseconds to return %d results\n", time, total_hit_counter);

}
