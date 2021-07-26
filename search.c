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
  uin_wtable[wcount]=NULL;
  return uin_wtable;
}

int is_vowel(char* word, int index)
{
  if(!isalpha(word[index]))
    return -1;
  char c = word[index];
  if(c=='a'||c=='e'||c=='i'||c=='o'||c=='u')
    return 1;
  if(!index)
    return 0;
  char d = word[index-1];
  if(index>0&&c == 'y'&&!(d=='a'||d=='e'||d=='i'||d=='o'||d=='u'))
    return 1;
  return 0;
}

int starc(char ch, char* word)
{
  int index = 0;
  while(word[index])
    index++;
  if(word[index-1]==ch)
    return index;
  return 0;
}

int starv(char* word)
{
  int index=0;
  while(word[index])
  {
    if(index!=0&&word[index+1]!=0&&is_vowel(word,index)==1)
      return 1;
    index++;
  }
  return 0;
}

int stard(char* word)
{
  int index = 0;
  while(word[index])
    index++;
  if(index<2)
    return 0;
  if(!is_vowel(word, index-1)&&!is_vowel(word,index-2))
    return 1;
  return 0;
}

int staro(char* word)
{
  int index = 0;
  while(word[index])
    index++;
  if(!is_vowel(word,index-1)&&word[index-1]!='w'&&word[index-1]!='x'&&word[index-1]!='y')
    if(is_vowel(word, index-2)==1)
      if(!is_vowel(word,index-3))
        return 1;
  return 0;
}

char* porter_stemmer(char* word)
{
  int index =0;
  int m = 0;
  int vflag = 0;
  int cflag = 0;
  while(word[index])
  {
    if(is_vowel(word,index)==-1)
      return word;
    if(is_vowel(word,index)==1)
      vflag=1;
    if(vflag==1&&!is_vowel(word,index))
      cflag = 1;
    if(vflag==1&&cflag==1)
    {
      m++;
      cflag=0;
      vflag=0;
    }
    index++;
  }
  printf("%s\n", word);
  if(index<=3)
    return word;

  if(word[index-1]=='s'&&word[index-2]=='e')//step 1-a
  {
      if(word[index-3]=='i'||(word[index-3]=='s'&&word[index-4]=='s'))
        for(int i = 1;i<=2;i++)
          word[--index]=0;
  }
  else if(word[index-1]=='s'&&word[index-2]!='s')
    word[--index]=0;
  puts(word);

  int patch = 0;
  if(m>0&&word[index-1]=='d'&&word[index-2]=='e'&&word[index-3]=='e')//step 1-b
    word[--index]=0;
  else if(starv(word)&&word[index-1]=='d'&&word[index-2]=='e')
  {
    patch = 1;
    for(int i = 1;i<=2;i++)
      word[--index]=0;
  }
  else if(starv(word)&&word[index-1]=='g'&&word[index-2]=='n'&&word[index-3]=='i')
  {
    patch = 1;
    for(int i = 1;i<=3;i++)
      word[--index]=0;
  }
  else if(word[index-1]=='s'&&word[index-2]!='s')
    word[--index]=0;
  puts(word);
  if(patch)
  {
    if((word[index-1]=='t'&&word[index-2]=='a')||(word[index-1]=='l'&&word[index-2]=='b')||(word[index-1]=='z'&&word[index-2]=='i'))
      word[index]='e';
    else if(stard(word)&&!(starc('l',word)||starc('s',word)||starc('z',word)))
      word[--index]=0;
    else if(word[index-1]=='s'&&word[index-2]!='s')
      word[--index]=0;
    else if(m==1&&staro(word))
      word[index]='e';
    puts(word);
  }

  if(starv(word)&&word[index-1]=='y')//step 1-c
    word[index-1]='i';

}
