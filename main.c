#include<stdio.h>
#include<string.h>
#include<time.h>

#include"databasehandling.h"
#include"search.h"

#define INPUT_LIMIT 500


int main()
{
  char c;
  long stop_db_size;//number of words in stop database
  char stop_db_path[]="SE-db/stop-words.txt";
  char index_path[]="SE-db/index.txt";
  char** stop_db_arr=import_stopdb_tomem(&stop_db_size,stop_db_path);
  long size_index;             //import index structure
  struct index_word* index = import_index_tomem(&size_index, index_path);

  char raw_uin_str[INPUT_LIMIT];
  char raw_uin_str_cp[INPUT_LIMIT];
  printf("Search for: ");
  fgets(raw_uin_str,INPUT_LIMIT,stdin);
  raw_uin_str[strcspn(raw_uin_str, "\n")] = 0;
  strcpy(raw_uin_str_cp, raw_uin_str);
  tolowerstr(raw_uin_str_cp);
  puts(raw_uin_str_cp);

  int kwsize;
  char** uin_wtable = extract_keywords(stop_db_size, stop_db_arr, raw_uin_str_cp,&kwsize);
  int i = 0;
  while(uin_wtable[i] != NULL)
    porter_stemmer(uin_wtable[i]);

}
