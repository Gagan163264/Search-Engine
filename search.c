#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include <stdlib.h>

#include"databasehandling.h"

#define WORD_SEPERATOR '|'
#define SENTENCE_SEPERATOR ';'
#define INPT_SEPERATOR " "


long long get_hash_key(char* word)
{
  int p = 31;
  long m = 1000000009;
  long long key = 0;
  long long mult =1;
  int index=0;
  char ch;
  int chr;
  while((ch=word[index])!=0)
  {
    if(!isalnum(ch))
    {
      index++;
      continue;
    }
    chr = (isdigit(ch))?'0':'a';
    key = (key+(ch-chr+1)*mult)%m;
    mult = (mult*p)%m;
    index++;
  }
  return key;
}

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

int bin_search_struct(long size, struct index_word* index_arr, char* cmp_str, int sel) //binary search in a array in struxt
{
  long mp;
  long long hkey;
  int left = 0;
  int right = size-1;
  if(sel<0)
    hkey = get_hash_key(cmp_str);
  if(right<0)
    right = 0;
  int cmp;
  int count = 0;
  int collisionstart;
  int collisionend;
  int cmpres;
  int cmpr;
  while(left<=right)
  {
    count++;
    mp = (left+right)/2;
    if(sel<0)
    {
      cmp = index_arr[mp].hash_key-hkey;
      cmpres = strcmp(index_arr[mp].word, cmp_str);
      if(cmp==0&&cmpres)
      {
        printf("\n\nCollision between %s and %s\n\n", index_arr[mp].word,cmp_str);
        if(cmpres>0)
        {
          collisionstart=mp-1;
          while(index_arr[collisionstart].hash_key==hkey)
          {
            cmpr = strcmp(index_arr[collisionstart].word,cmp_str);
            if(!cmpr)
                mp=collisionstart;
            if(cmpr<0)
            {
              mp=-(collisionstart+2);
              break;
            }
            collisionstart--;
            }
          }
        else
        {
          collisionend=mp+1;
          while(index_arr[collisionend].hash_key==hkey)
          {
            cmpr = strcmp(index_arr[collisionend].word,cmp_str);
            if(!cmpr)
                mp=collisionstart;
            if(cmpr>0)
            {
              mp=-(collisionstart);
              break;
            }
            collisionstart--;
          }
        }
      }
    }
    else
      cmp = strcmp(index_arr[sel].doc_data[mp].docname, cmp_str);
    //if(sel>=0)
      //printf("\ninarr(%s||%s||%d||%d||%ld)\n", cmp_str,index_arr[sel].doc_data[mp].docname,cmp,sel,mp);
    if(cmp == 0)
      return mp;
    if(cmp>0)
      right = mp-1;
    else
      left = mp+1;
  }
  if(cmp<0)
    return -mp-2;
  return -mp-1;
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


//-----------------------------------------porter stemmer-----------------------------------------------------------------------------------------
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

int getm(char* word, int end)
{
  int m = 0;
  int vflag = 0;
  int cflag = 0;
  int indx =0;
  while(word[indx])
    indx++;
  if((indx-end)<=0)
    return 0;
  int index = 0;
  while(index<(indx-end))
  {
    if(is_vowel(word,index)==-1)
      return -1;
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
  if((index+end)!=indx)
    return 0;
  return m;
}

int starc(char ch, char* word,int end)
{
  int indx = 0;
  while(word[indx])
    indx++;
  if((indx-end)<=0)
    return 0;
  int index = 0;
  while(word[index]&&index<(indx-end))
    index++;
  if(word[index-1]==ch)
    return index;
  if((index+end)!=indx)
    return 0;
  return 0;
}

int starv(char* word,int end)
{
  int indx = 0;
  while(word[indx])
    indx++;
  if((indx-end)<=0)
    return 0;
  int index = 0;
  while(word[index]&&index<(indx-end))
  {
    if(index!=0&&word[index+1]!=0&&is_vowel(word,index)==1)
      return 1;
    index++;
  }
  return 0;
}

int stard(char* word, int end)
{
  int indx = 0;
  while(word[indx])
    indx++;
  if((indx-end)<=0)
    return 0;
  int index = 0;
  while(word[index]&&index<(indx-end))
    index++;
  if(index<2)
    return 0;
  if(!is_vowel(word, index-1)&&!is_vowel(word,index-2)&&word[index-1]==word[index-2])
    return 1;
  return 0;
}

int staro(char* word, int end)
{
  int vflag = 0;
  int indx = 0;
  while(word[indx])
    indx++;
  if((indx-end)<=0)
    return 0;
  int index = indx-end-1;
  /*if(!is_vowel(word,index)&&word[index]!='w'&&word[index]!='x'&&word[index]!='y')
  {
    while(index>=0)
    {
      if(is_vowel(word, index)==1)
        vflag = 1;
      if(vflag&&!is_vowel(word,index))
        return 1;
      index--;
    }
  }*/
  if(!is_vowel(word,index)&&word[index]!='w'&&word[index]!='x'&&word[index]!='y'&&is_vowel(word,index-1)&&!is_vowel(word,index-2))
    return 1;
  return 0;
}

char* porter_stemmer(char* word)
{
  if(getm(word, 0)<0)
    return word;
  int index = 0;
  while(word[index])
    index++;
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

  int patch = 0;
  if(getm(word,3)>0&&word[index-1]=='d'&&word[index-2]=='e'&&word[index-3]=='e')//step 1-b
    word[--index]=0;
  else if(starv(word,2)&&word[index-1]=='d'&&word[index-2]=='e')
  {
    patch = 1;
    for(int i = 1;i<=2;i++)
        word[--index]=0;
  }
  else if(starv(word,3)&&word[index-1]=='g'&&word[index-2]=='n'&&word[index-3]=='i')
  {
    patch = 1;
    for(int i = 1;i<=3;i++)
      word[--index]=0;
  }
  else if(word[index-1]=='s'&&word[index-2]!='s')
    word[--index]=0;
  if(patch)
  {
    if((word[index-1]=='t'&&word[index-2]=='a')||(word[index-1]=='l'&&word[index-2]=='b')||(word[index-1]=='z'&&word[index-2]=='i'))
      word[index]='e';
    else if(stard(word,0)&&!(starc('l',word,0)||starc('s',word,0)||starc('z',word,0)))
      word[--index]=0;
    else if(word[index-1]=='s'&&word[index-2]!='s')
      word[--index]=0;
    else if(getm(word,0)==1&&staro(word,0))
      word[index]='e';
  }

  if(starv(word,1)&&word[index-1]=='y')//step 1-c
    word[index-1]='i';


  if(getm(word,7)&&word[index-1]=='l'&&word[index-2]=='a'&&word[index-3]=='n'&&word[index-4]=='o'&&word[index-5]=='i'&&word[index-6]=='t'&&word[index-7]=='a')//step 2
  {
    for(int i = 1;i<=5;i++)
      word[--index]=0;
    word[index]='e';
  }
  else if(getm(word,6)&&word[index-1]=='l'&&word[index-2]=='a'&&word[index-3]=='n'&&word[index-4]=='o'&&word[index-5]=='i'&&word[index-6]=='t')
    for(int i = 1;i<=2;i++)
      word[--index]=0;
  else if(getm(word,4)&&word[index-1]=='i'&&word[index-2]=='c'&&word[index-3]=='n'&&(word[index-4]=='a'||word[index-4]=='e'))
    word[index-1]='e';
  else if(getm(word,4)&&word[index-1]=='r'&&word[index-2]=='e'&&word[index-3]=='z'&&word[index-4]=='i')
      word[--index]=0;
  else if(getm(word,4)&&word[index-1]=='i'&&word[index-2]=='l'&&word[index-3]=='b'&&word[index-4]=='a')
      word[--index]='e';
  else if(getm(word,4)&&word[index-1]=='i'&&word[index-2]=='l'&&word[index-3]=='l'&&word[index-4]=='a')
   for(int i = 1;i<=2;i++)
      word[--index]=0;
  else if(getm(word,5)&&word[index-1]=='i'&&word[index-2]=='l'&&word[index-3]=='t'&&word[index-4]=='n'&&word[index-5]=='e')
   for(int i = 1;i<=2;i++)
      word[--index]=0;
  else if(getm(word,3)&&word[index-1]=='i'&&word[index-2]=='l'&&word[index-3]=='e')
   for(int i = 1;i<=2;i++)
      word[--index]=0;
  else if(getm(word,5)&&word[index-1]=='i'&&word[index-2]=='l'&&word[index-3]=='s'&&word[index-4]=='u'&&word[index-5]=='o')
   for(int i = 1;i<=2;i++)
      word[--index]=0;
  else if(getm(word,7)&&word[index-1]=='n'&&word[index-2]=='o'&&word[index-3]=='i'&&word[index-4]=='t'&&word[index-5]=='a'&&word[index-6]=='z'&&word[index-7]=='i')
    {
      for(int i = 1;i<=5;i++)
        word[--index]=0;
      word[index]='e';
    }
  else if(getm(word,5)&&word[index-1]=='n'&&word[index-2]=='o'&&word[index-3]=='i'&&word[index-4]=='t'&&word[index-5]=='a')
    {
      for(int i = 1;i<=3;i++)
        word[--index]=0;
      word[index]='e';
    }
  else if(getm(word,4)&&word[index-1]=='r'&&word[index-2]=='o'&&word[index-3]=='t'&&word[index-4]=='a')
    {
      for(int i = 1;i<=2;i++)
        word[--index]=0;
      word[index]='e';
    }
  else if(getm(word,5)&&word[index-1]=='m'&&word[index-2]=='s'&&word[index-3]=='i'&&word[index-4]=='l'&&word[index-5]=='a')
      for(int i = 1;i<=3;i++)
        word[--index]=0;
  else if(getm(word,7)&&word[index-1]=='s'&&word[index-2]=='s'&&word[index-3]=='e'&&word[index-4]=='n'&&((word[index-5]=='e'&&word[index-6]=='v'&&word[index-7]=='i')||(word[index-5]=='l'&&word[index-6]=='u'&&word[index-7]=='f')||(word[index-5]=='s'&&word[index-6]=='u'&&word[index-7]=='o')))
      for(int i = 1;i<=4;i++)
        word[--index]=0;
  else if(getm(word,5)&&word[index-1]=='i'&&word[index-2]=='t'&&word[index-3]=='i'&&word[index-4]=='l'&&word[index-5]=='a')
   for(int i = 1;i<=3;i++)
      word[--index]=0;
  else if(getm(word,5)&&word[index-1]=='i'&&word[index-2]=='t'&&word[index-3]=='i'&&word[index-4]=='v'&&word[index-5]=='i')
  {
    for(int i = 1;i<=3;i++)
      word[--index]=0;
    word[index]='e';
  }
  else if(getm(word,6)&&word[index-1]=='i'&&word[index-2]=='t'&&word[index-3]=='i'&&word[index-4]=='l'&&word[index-5]=='i'&&word[index-6]=='b')
  {
    for(int i = 1;i<=5;i++)
      word[--index]=0;
    word[index]='l';
    word[index+1]='e';
  }

  if(getm(word,5)&&word[index-1]=='e'&&word[index-2]=='t'&&word[index-3]=='a'&&word[index-4]=='c'&&word[index-5]=='i')//step 3
    for(int i = 1;i<=3;i++)
      word[--index]=0;
  else if(getm(word,5)&&word[index-1]=='e'&&word[index-2]=='v'&&word[index-3]=='i'&&word[index-4]=='t'&&word[index-5]=='a')
    for(int i = 1;i<=5;i++)
      word[--index]=0;
  else if(getm(word,5)&&word[index-1]=='e'&&word[index-2]=='z'&&word[index-3]=='i'&&word[index-4]=='l'&&word[index-5]=='a')
    for(int i = 1;i<=3;i++)
      word[--index]=0;
  else if(getm(word,5)&&word[index-1]=='i'&&word[index-2]=='t'&&word[index-3]=='i'&&word[index-4]=='c'&&word[index-5]=='i')
    for(int i = 1;i<=3;i++)
      word[--index]=0;
  else if(getm(word,4)&&word[index-1]=='l'&&word[index-2]=='a'&&word[index-3]=='c'&&word[index-4]=='i')
    for(int i = 1;i<=2;i++)
      word[--index]=0;
  else if(getm(word,3)&&word[index-1]=='l'&&word[index-2]=='u'&&word[index-3]=='f')
    for(int i = 1;i<=3;i++)
      word[--index]=0;
  else if(getm(word,4)&&word[index-1]=='s'&&word[index-2]=='s'&&word[index-3]=='e'&&word[index-4]=='n')
    for(int i = 1;i<=4;i++)
      word[--index]=0;

  if(getm(word,2)>1&&((word[index-1]=='l'&&word[index-2]=='a')||(word[index-1]=='r'&&word[index-2]=='e')||(word[index-1]=='c'&&word[index-2]=='i')))//step 4
    for(int i = 1;i<=2;i++)
      word[--index]=0;
  else if(getm(word,4)>1&&word[index-1]=='e'&&word[index-2]=='c'&&word[index-3]=='n'&&(word[index-4]=='a'||word[index-4]=='e'))
    for(int i = 1;i<=4;i++)
      word[--index]=0;
  else if(getm(word,4)>1&&word[index-1]=='e'&&word[index-2]=='l'&&word[index-3]=='b'&&(word[index-4]=='a'||word[index-4]=='i'))
    for(int i = 1;i<=4;i++)
      word[--index]=0;
  else if(getm(word,3)>1&&word[index-1]=='t'&&word[index-2]=='n'&&word[index-3]=='s')
    for(int i = 1;i<=3;i++)
      word[--index]=0;
  else if(getm(word,5)>1&&word[index-1]=='t'&&word[index-2]=='n'&&word[index-3]=='e'&&word[index-4]=='m'&&word[index-5]=='e')
    for(int i = 1;i<=5;i++)
      word[--index]=0;
  else if(getm(word,4)>1&&word[index-1]=='t'&&word[index-2]=='n'&&word[index-3]=='e'&&word[index-4]=='m')
    for(int i = 1;i<=4;i++)
      word[--index]=0;
  else if(getm(word,3)>1&&word[index-1]=='t'&&word[index-2]=='n'&&word[index-3]=='e')
    for(int i = 1;i<=3;i++)
      word[--index]=0;
  else if(getm(word,3)>1&&(starc('s',word,3)||starc('t',word,3))&&word[index-1]=='n'&&word[index-2]=='o'&&word[index-3]=='i')
    for(int i = 1;i<=3;i++)
      word[--index]=0;
  else if(getm(word,3)>1&&word[index-1]=='s'&&word[index-2]=='u'&&word[index-3]=='o')
    for(int i = 1;i<=3;i++)
      word[--index]=0;
  else if(getm(word,2)>1&&word[index-1]=='u'&&word[index-2]=='o')
    for(int i = 1;i<=2;i++)
      word[--index]=0;
  else if(getm(word,3)>1&&word[index-1]=='m'&&word[index-2]=='s'&&word[index-3]=='i')
    for(int i = 1;i<=3;i++)
      word[--index]=0;
  else if(getm(word,3)>1&&word[index-1]=='e'&&word[index-2]=='t'&&word[index-3]=='a')
    for(int i = 1;i<=3;i++)
      word[--index]=0;
  else if(getm(word,3)>1&&word[index-1]=='i'&&word[index-2]=='t'&&word[index-3]=='i')
    for(int i = 1;i<=3;i++)
      word[--index]=0;
  else if(getm(word,3)>1&&word[index-1]=='e'&&(word[index-2]=='v'||word[index-2]=='z')&&word[index-3]=='i')
    for(int i = 1;i<=3;i++)
      word[--index]=0;


  if(getm(word,1)>1&&word[index-1]=='e')//step 5-a
      word[--index]=0;
  else if(getm(word,1)==1&&!staro(word, 1)&&word[index-1]=='e')
      word[--index]=0;

  if(getm(word,0)>1&&starc('d',word,0)&&starc('l',word,0))//step 5-B
      word[--index]=0;

  return word;
}
