#include <stdio.h>
#include <dirent.h>
#include<string.h>
#include<stdlib.h>
#include"databasehandling.h"

int main(void)
{
  FILE *fptr;
  int check_txt, path_append;
  char c;
  char* dot;
  struct dirent *de;
  char fpath[100]="source/";
  char index_path[]="SE-db/index.txt";
  DIR *dr = opendir("source");
  if (dr == NULL)  // opendir returns NULL if couldn't open directory
  {
      printf("Could not open current directory" );
      return 0;
  }
  while ((de = readdir(dr)) != NULL)
  {
    check_txt = 0;   //check if file has extension of 'txt'
    while(de->d_name[check_txt++] != '.');
    dot = de->d_name+check_txt;
    if(strcmp(dot, "txt"))
      continue;

    path_append = 0; //append database directory name to path
    check_txt = 0;
    while(fpath[path_append++] != '/');
    while(de->d_name[check_txt] != 0)
        fpath[path_append++]=de->d_name[check_txt++];
    fpath[path_append]=0;

    /*fptr = fopen(fpath, "r");
    if (fptr == NULL)
    {
        printf("Cannot open file \n");
        exit(0);
    }
    c = fgetc(fptr);
    while (c != EOF)
    {
        printf ("%c", c);
        c = fgetc(fptr);
    }
    fclose(fptr);
      */
  }

  long size;
  struct index_word* index = import_index_tomem(&size, index_path);
  printf("\n\n");
  printf("%s||%d||(%s||%d)\n", index[1].word, index[1].freq_across_docs, index[1].doc_data[1].docname, index[1].doc_data[1].freq);


  closedir(dr);
  return 0;
}
