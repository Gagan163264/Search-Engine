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

  DIR *dr = opendir("source");
  if (dr == NULL)  // opendir returns NULL if couldn't open directory
  {
      printf("Could not open current directory" );
      return 0;
  }


  long size_index;             //import index structure
  struct index_word* index = import_index_tomem(&size_index, index_path);
  printf("\n\n");
  printf("%s||%d||(%s||%d)\n", index[1].word, index[1].freq_across_docs, index[1].doc_data[1].docname, index[1].doc_data[1].freq);

  //vars used inside while
  long size_d;
  int findex = 0;
  char* sep;
  char sepsp[] = " ";
  char sepcm[] = ",";
  char* raw_word;
  char* rword;
  char* word;
  int endword = 0;
  int beginword = 0;
  int nlcount = 0;
  int strindex;
  int laststrindex;
  
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
    while(rword != NULL )
    {
      endword =0;
      beginword = 0;
      nlcount = 0;
      while(rword[endword]!=0)
      {
        if(rword[endword] == '\n')
          nlcount++;
        endword++;
      }
      endword =0;
      strindex = 0;
      laststrindex = 0;
      while(nlcount>=0)
      {
        while(rword[strindex]!='\n'&&rword[strindex]!=0)
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

        while(raw_word[endword]!=0)
          endword++;
        while((raw_word[beginword]!=0)&&(!isalnum(raw_word[beginword])))
          beginword++;
        if(beginword==endword)
          break;
        endword--;
        while(((endword)>=0)&&(!isalnum(raw_word[endword])))
          endword--;
        word = (char*)malloc((endword-beginword+2)*sizeof(char));
        for(int i = 0;i<=endword;i++)
            word[i]=tolower(raw_word[beginword+i]);
        word[endword-beginword+1]=0;

        printf("%s\n\n", word);  //whatever with word here

        free(word);
        free(raw_word);
      }
      rword = strtok(NULL, sep);
    }
    free(fle);
  }

  closedir(dr);
  return 0;
}
