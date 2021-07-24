#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include <stdlib.h>

#include"databasehandling.h"

#define WORD_SEPERATOR '|'
#define SENTENCE_SEPERATOR ';'
#define INPT_SEPERATOR " "

void tolowerstr(char arr[]) //convert array to lowercase
{
  int i = 0;
  while(arr[i] != 0)
  {
    arr[i] = tolower(arr[i]);
    i++;
  }
}

int strcmp_bin(int n, char** db, char* word)
{
  return strcmp(db[n], word);
}

int bin_search(long size, char** db_arr, char cmp_str[], int(*cmp_fn)(int, char**, char*)) //binary search in a 2d array
{
  long mp;
  int left = 0;
  int right = size-1;
  int cmp;
  int count = 0;
  while(left<=right)
  {
    count++;
    mp = (left+right)/2;
    cmp = (*cmp_fn)(mp, db_arr, cmp_str);
    if(cmp == 0)
      return -mp-1;
    if(cmp>0)
      right = mp-1;
    else
      left = mp+1;
  }
  return mp;
}

char** extract_keywords(long stop_db_size, char** stop_db_arr, char raw_uin_str[])
{
  int wcount = 0;
  char *word;
  char spacer[] = INPT_SEPERATOR;
  int word_count = 10;
  char **uin_wtable = (char **)malloc(word_count * sizeof(char *));
  word = strtok(raw_uin_str, spacer);
  while(word != NULL)
  {
    if(bin_search(stop_db_size, stop_db_arr, word, strcmp_bin)>=0)    //search for word in stop-word database
    {
      uin_wtable[wcount]=(char *)calloc(30, sizeof(char));
      for(int i = 0;word[i]!=0;i++)
      {
        if(word[i]>64&&word[i]<91)
          uin_wtable[wcount][i]=tolower(word[i]);
        else if(word[i]>96&&word[i]<123)
          uin_wtable[wcount][i]=word[i];
        else
        uin_wtable[wcount][i]=' ';
      }
      wcount++;
      if(wcount>word_count)
      {
        word_count = 2*word_count;
        uin_wtable = (char **)realloc(uin_wtable,  word_count* sizeof(char *));
      }
    }
    word = strtok(NULL, spacer);
  }

  if(wcount == 0)                                       // if no keywords found, copy stop words(failure point)
  {
    word = strtok(raw_uin_str, spacer);
    while(word != NULL)
    {
      uin_wtable[wcount]=(char *)calloc(50, sizeof(char));
      strcpy(uin_wtable[wcount++], word);
      word = strtok(NULL, spacer);
    }
  }

  if(wcount<word_count)                                 // resize array to fit data
  {
    uin_wtable = (char **)realloc(uin_wtable,  (wcount+1)* sizeof(char *));
  }
  uin_wtable[wcount+1]=NULL;
  return uin_wtable;
}


char* porter_stemmer(char* word)
{


}
