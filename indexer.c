#include <stdio.h>
#include <dirent.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>

#include"databasehandling.h"
#include"search.h"


int main(void)
{
  FILE *fptr;  //declarations
  int check_txt, path_append;
  char c;
  char* dot;
  struct dirent *de;
  char fpath[100]="source/";
  char index_path[]="SE-db/index.txt";
  char stop_db_path[]="SE-db/stop-words.txt";

  long stop_size;
  char** stop_db_arr = import_stopdb_tomem(&stop_size, stop_db_path);

  DIR *dr = opendir("source");
  if (dr == NULL)  // opendir returns NULL if couldn't open directory
  {
      printf("Could not open current directory" );
      return 0;
  }


  long size_index;             //import index structure
  struct index_word* index = import_index_tomem(&size_index, index_path);
  printf("%s||%d||(%s||%d)\n", index[1].word, index[1].freq_across_docs, index[1].doc_data[1].docname, index[1].doc_data[1].freq);
  printf("\n\n");


  //vars used inside while
  long size_d;
  int findex = 0;
  char* sep;
  char sepsp[] = " ";
  char sepcm[] = ",";
  char* raw_word;
  char* rword;
  char* oword;
  char* word;
  int endword = 0;
  int beginword = 0;
  int nlcount = 0;
  int strindex;
  int laststrindex;
  int hypindex;
  int hypcount;
  int startvar;
  int endvar;
  int contrindex;
  int contadd;
  int contflag;
  int wordsize;
  int pass;

  while ((de = readdir(dr)) != NULL)
  {
    check_txt = 0;   //check if file has extension of 'txt' or 'csv'
    while(de->d_name[check_txt++] != '.');
    dot = de->d_name+check_txt;
    if(strcmp(dot, "txt")&&strcmp(dot, "csv"))
      continue;

    if(!strcmp(dot,"txt"))//select appropriate word seperators for respective file
      sep = sepsp;
    if(!strcmp(dot,"csv"))
      sep = sepcm;

    path_append = 0; //append database directory name to path
    check_txt = 0;
    while(fpath[path_append++] != '/');
    while(de->d_name[check_txt] != 0)
        fpath[path_append++]=de->d_name[check_txt++];
    fpath[path_append]=0;

    char* fle = importdb_tomem(&size_d, fpath);
    rword = strtok(fle, sep);

    char ch;
    while(rword != NULL )
    {
      endword =0;
      beginword = 0;
      nlcount = 0;
      while((ch = rword[endword]))
      {
        if(ch=='+'||ch=='<'||ch=='>'||ch=='^')//https://docs.oracle.com/cd/E29584_01/webhelp/mdex_basicDev/src/cbdv_searchchar_indexing_non-alphanumeric_characters.html
          rword[endword]='\n';
        if(rword[endword]=='.'&&rword[endword+1]=='.')//detect ellipses
        {
          beginword=endword;
          while(rword[endword++]=='.')
          rword[beginword+((endword-beginword)/2)]='\n';
          endword = beginword;
        }
        if(rword[endword] == '\n')//count newlines
          nlcount++;
        endword++;
      }
      beginword = 0;
      endword =0;
      strindex = 0;
      laststrindex = 0;
      while(nlcount>=0)
      {
        while(rword[strindex]!='\n'&&rword[strindex])
          strindex++;
        if(rword[strindex]=='\n'&&rword[strindex+1]==0)
          nlcount=-1;
        raw_word = (char*)malloc((strindex-laststrindex+1)*sizeof(char));
        raw_word[strindex-laststrindex]=0;
        for(int i = 0;i<(strindex-laststrindex);i++)
          raw_word[i]=rword[laststrindex+i];
        laststrindex = strindex+1;
        if(rword[strindex]=='\n')
          nlcount--;
        if(rword[strindex]==0)
          nlcount=-1;
        strindex++;

        while(raw_word[endword])
          endword++;
        while((raw_word[beginword])&&(!isalnum(raw_word[beginword]))&&strcmp(sep,"csv"))
          beginword++;
        if(beginword==endword)
          break;
        endword--;
        while(((endword)>=0)&&(!isalnum(raw_word[endword]))&&strcmp(sep,"csv"))
          endword--;
        hypcount = 0;
        hypindex = 0;
        for(int i = 0;i<=endword;i++)
          if(raw_word[beginword+i]=='-'&&strcmp(sep,"csv"))
          {
            hypcount++;
            hypindex=i;
          }
        if(hypcount!=1)
          hypindex=endword;
        hypcount = (hypcount==1)?3:1;
        while(hypcount>0)
        {
          if(hypcount==1)
          {
            startvar=beginword;
            endvar=endword;
          }
          else if(hypcount==3)
          {
            startvar = beginword;
            endvar=hypindex-1;
          }
          else if(hypcount==2)
          {
            startvar = hypindex+1;
            endvar=endword;
          }

          pass = 0;
          contflag = 0;
          contadd = 0;
          contrindex = 0;
          for(int i = endvar;i>=0;i--)
            if(raw_word[startvar+i]=='\''&&strcmp(sep, "csv"))
              contrindex=i;

          while(contrindex>=0)//seperate contractions into different words
          {
            if(!contflag&&contrindex&&raw_word[startvar+contrindex-1]=='n'&&(endvar>=startvar+contrindex+1)&&raw_word[startvar+contrindex+1]=='t')//contraction 1
            {
              endvar=startvar+contrindex-2;
              contflag=1;
            }
            else if(!contflag&&contrindex&&(endvar>=startvar+contrindex+2)&&raw_word[startvar+contrindex+1]=='v'&&raw_word[startvar+contrindex+2]=='e')//contraction 2
              contflag=2;
            else if(!contflag&&contrindex&&(endvar>=startvar+contrindex+1)&&raw_word[startvar+contrindex+1]=='s')//contraction 3
              contflag=3;
            else if(!contflag&&contrindex&&(endvar>=startvar+contrindex+2)&&raw_word[startvar+contrindex+1]=='r'&&raw_word[startvar+contrindex+2]=='e')//contraction 4
              contflag=4;
            else if(!contflag&&contrindex&&(endvar>=startvar+contrindex+1)&&raw_word[startvar+contrindex+1]=='d')//contraction 5
              contflag=5;
            else if(!contflag&&contrindex&&(endvar>=startvar+contrindex+2)&&raw_word[startvar+contrindex+1]=='l'&&raw_word[startvar+contrindex+2]=='l')//contraction 6
              contflag=6;
            else if(!contflag&&contrindex&&(endvar>=startvar+contrindex+1)&&raw_word[startvar+contrindex+1]=='m')//contraction 7
              contflag=7;
            else{}

            if(contrindex)
            {
              if(contflag!=1)
                endvar=startvar+contrindex-1;
              contrindex=1;
              if(contflag==5&&!(pass++))
                contrindex=2;
            }
            if(!contrindex&&contflag==1)
             {
               wordsize=4;
               word = (char*)malloc(wordsize*sizeof(char));
               strcpy(word,"not");
               word[wordsize-1]=0;
             }
             else if(!contrindex&&contflag==2)
             {
               wordsize=5;
               word = (char*)malloc(wordsize*sizeof(char));
               strcpy(word,"have");
               word[wordsize-1]=0;
             }
             else if(!contrindex&&contflag==3)
             {
               wordsize=3;
               word = (char*)malloc(wordsize*sizeof(char));
               strcpy(word,"is");
               word[wordsize-1]=0;
             }
             else if(!contrindex&&contflag==4)
             {
               wordsize=4;
               word = (char*)malloc(wordsize*sizeof(char));
               strcpy(word,"are");
               word[wordsize-1]=0;
             }
             else if(contrindex!=2&&contflag==5)
             {
               if(!contrindex)
               {
               wordsize=6;
               word = (char*)malloc(wordsize*sizeof(char));
               strcpy(word,"would");
               word[wordsize-1]=0;
             }
              if(contrindex==1)
              {
                wordsize=4;
                word = (char*)malloc(wordsize*sizeof(char));
                strcpy(word,"had");
                word[wordsize-1]=0;
              }
             }
             else if(!contrindex&&contflag==6)
             {
               wordsize=5;
               word = (char*)malloc(wordsize*sizeof(char));
               strcpy(word,"will");
               word[wordsize-1]=0;
             }
             else if(!contrindex&&contflag==7)
             {
               wordsize=3;
               word = (char*)malloc(wordsize*sizeof(char));
               strcpy(word,"am");
               word[wordsize-1]=0;
             }

            else
            {
              word = (char*)malloc((endvar-startvar+2)*sizeof(char));
              for(int i = 0;i<=endvar;i++)
                  word[i]=tolower(raw_word[startvar+i]);
              word[endvar-startvar+1]=0;
            }
            if(word[0])
            {
              if(bin_search(stop_size,stop_db_arr,word,strcmp_bin)>=0)
              {
                porter_stemmer(word);
                printf("||%s\n\n", word);
              }
            }
            free(word);
            contrindex--;
          }
          hypcount--;
        }

        free(raw_word);
      }
      rword = strtok(NULL, sep);
    }
    free(fle);
  }

  closedir(dr);
  return 0;
}
