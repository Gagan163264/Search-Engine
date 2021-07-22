#include<stdio.h>
#include<string.h>
#include<time.h>

#include"databasehandling.h"
#include"search.h"

#define INPUT_LIMIT 200


int main()
{
  char c;
  long stop_db_size;//number of words in stop database
  char stop_db_path[]="SE-db/stop-words.txt";
  char** stop_db_arr=import_stopdb_tomem(&stop_db_size,stop_db_path);

  char raw_uin_str[INPUT_LIMIT]= "Coordinates to tHe hidden temple, where we shall find treasure";
  char raw_uin_str_cp[INPUT_LIMIT];
  strcpy(raw_uin_str_cp, raw_uin_str);
  tolowerstr(raw_uin_str_cp);
  puts(raw_uin_str_cp);

  char** uin_wtable = extract_keywords(stop_db_size, stop_db_arr, raw_uin_str_cp);
  int i = 0;
  while(uin_wtable[i] != NULL)
    puts(uin_wtable[i++]);
}
