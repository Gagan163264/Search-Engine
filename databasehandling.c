#include<stdio.h>
#include <stdlib.h>
#include<string.h>
#include"databasehandling.h"

#define DATABASE "database.txt"


/*Function takes long pointer(used as size output) and char pointer(contains database path) to give char pointer of file contents in RAM*/
char* importdb_tomem(long *database_size, char *databasedb)
{
  FILE *dbfile = fopen(databasedb, "r");
  fseek(dbfile, 0L, SEEK_END);
  long fsize = ftell(dbfile);
  fseek(dbfile, 0, SEEK_SET);
  char *database = malloc(fsize + 1);
  fread(database, 1, fsize, dbfile);
  fclose(dbfile);
  database[fsize] = 0;
  *database_size = fsize;
  return database;
}


char** import_stopdb_tomem(long *database_size, char *databasedb)
{
  char* database = importdb_tomem(database_size, databasedb);
  int line_count = 0;
  int line_index = 0;
  int max_size = 0;
  int letter_counter = 0;
  long counter = 0;
  char c;
  while((c = database[counter++]) != 0)
  {
    if(c == '\n')
    {
      line_count++;
      if(letter_counter>max_size)
        max_size=letter_counter;
      letter_counter=0;
    }
    else
      letter_counter++;
  }
  max_size++; //to add 0 as termination character
  *database_size=line_count;
  char **stop_db_arr = (char **)malloc(line_count * sizeof(char *));
  for (int i=0; i<line_count; i++)
         stop_db_arr[i] = (char *)malloc(max_size * sizeof(char));

  letter_counter = 0;
  counter = 0;
  while((c = database[counter++]) != 0)
  {
    if(c == '\n')
    {
      while(letter_counter<max_size)
          stop_db_arr[line_index][letter_counter++]=0;
      letter_counter = 0;
      line_index++;
    }
    else
    stop_db_arr[line_index][letter_counter++]=c;
  }
  free(database);
  return stop_db_arr;
}


struct index_word* import_index_tomem(long *database_size, char *databasedb)
{
  char* database = importdb_tomem(database_size, databasedb);
  int indexc = 0;
  char c;
  int elementcount = 0;
  while((c=database[indexc++])!=0)
    if(c=='\n')
      elementcount++;
  *database_size=elementcount;
  struct index_word* index = (struct index_word*)malloc((elementcount+1) * sizeof(struct index_word));
  index[elementcount].word=NULL;
  indexc = 0;
  long indx = 0;
  int sepcount = 0;
  int indexarr = 0;
  int elementcounter = 0;
  int element2counter = 0;
  int lastsep = 0;
  int substructctr;
  while((c = database[indx++])!=0)
  {
    if(c == '\n'|| indx==1)
    {
      if(database[indx]==0)
        break;
      indexarr = 0;
      if(c=='\n')
        indexc++;
      substructctr = -1;
      elementcounter = 0;
      element2counter = 0;
      sepcount = 0;
      for(int i = indx;(database[i]!='\n' && database[i]!= 0);i++)//declare structure
        if(database[i]=='|')
          sepcount++;
      index[indexc].doc_data = (struct freq_per_doc*)calloc((sepcount+2)/3,sizeof(struct freq_per_doc));
      index[indexc].doc_data[(sepcount-1)/3].orword=NULL;
      sepcount = 0;
      for(int i = indx;(database[i]!='\n' && database[i]!= 0);i++)//initialize structure
        if(database[i]=='|')
        {
          sepcount++;
          if(sepcount == 1)
          {
            index[indexc].hash_key = 0;
            index[indexc].word = (char*)calloc(i-indx+1, sizeof(char));
          }
          if(sepcount>2&&(sepcount%3)== 0)
          {
            index[indexc].doc_data[element2counter].freq=0;
            index[indexc].doc_data[element2counter].orword=(char*)calloc((i-lastsep),sizeof(char));
          }
          if(sepcount>2&&((sepcount-1)%3)== 0)
          {
            index[indexc].doc_data[element2counter].docname=(char*)calloc((i-lastsep),sizeof(char));
            element2counter++;
          }
          lastsep = i;
        }
      element2counter=0;
      if((sepcount-1)%3 != 0)
      {
        printf("Database error at line %d", indexc+2);
        return NULL;
      }
      if(c=='\n')
        continue;
    }
    if(c == '|')
    {
      indexarr = 0;
      elementcounter++;
      if((elementcounter+1)%3 == 0)
      {
        element2counter=0;
          substructctr++;
      }
      else if(elementcounter%3 == 0)
        element2counter=1;
      else
        element2counter = 2;
      continue;
    }
    if(elementcounter == 0)
      index[indexc].word[indexarr++]=c;
    if(elementcounter == 1)
      index[indexc].hash_key=10*index[indexc].hash_key+c-'0';
    if(elementcounter >= 2)
    {
      if(element2counter == 0)
        index[indexc].doc_data[substructctr].orword[indexarr++]=c;
      if(element2counter == 1)
        index[indexc].doc_data[substructctr].docname[indexarr++]=c;
      if(element2counter == 2)
      index[indexc].doc_data[substructctr].freq=10*(index[indexc].doc_data[substructctr].freq)+c-'0';
    }
  }
  free(database);
  return index;
}

int reversenum(int num)
{
  int rev_num = 0;
  while (num > 0) {
      rev_num = rev_num * 10 + num % 10;
      num = num / 10;
  }
  return rev_num;
}

 int export_index(struct index_word* index, long index_size, char* databasedb)
 {
   char db_backup[]="SE-db/index-backup.txt";
   FILE *dbfile = fopen(databasedb, "r");
   FILE *dbbfile = fopen(db_backup, "w");
   if(dbfile==NULL||dbbfile==NULL)
    return 1;
   char c;
   while ((c = fgetc(dbfile))!= EOF)
     fputc(c, dbbfile);
  fclose(dbfile);
  fclose(dbbfile);
  dbfile = fopen(databasedb, "w");
  fseek(dbfile, 0, SEEK_SET);

  int datai = 0;
  int ind = 0;
  int num = 0;
  int tmpn = 0;
  while(index[ind].word!=NULL)
  {
    fputs(index[ind].word, dbfile);
    fputc('|',dbfile);
    tmpn = index[ind].hash_key;
    num = reversenum(index[ind].hash_key);
    while(num>0)
    {
      fputc((num%10)+'0',dbfile);
      num = num/10;
    }
    while(tmpn%10==0)
    {
      fputc('0',dbfile);
      tmpn = tmpn/10;
    }
    fputc('|',dbfile);
    datai = 0;
    while(index[ind].doc_data[datai].orword!=NULL)
    {
      fputs(index[ind].doc_data[datai].orword, dbfile);
      fputc('|',dbfile);
      fputs(index[ind].doc_data[datai].docname, dbfile);
      fputc('|',dbfile);
      tmpn = index[ind].doc_data[datai].freq;
      num = reversenum(index[ind].doc_data[datai].freq);
      while(num>0)
      {
        fputc((num%10)+'0',dbfile);
        num = num/10;
      }
      while(tmpn%10==0)
      {
        fputc('0',dbfile);
        tmpn = tmpn/10;
      }
      if(index[ind].doc_data[datai+1].orword!=NULL)
        fputc('|',dbfile);
      datai++;
    }
    fputc('\n',dbfile);
    ind++;
  }
  fclose(dbfile);
  return 0;
 }

 struct docdet* import_dbindex(long* size, char* path)
 {
   long dbsize;
   long num;
   char* database = importdb_tomem(&dbsize, path);
   long i;
   while(i<dbsize)
    if(database[i++]=='\n')
      num++;
  *size = num
  struct docdet* dbindex = (struct docdet*)malloc((num+1)*sizeof(struct docdet));
  dbindex[num].name = NULL;
  i = 0;
  char ch;
  int wsize;
  int line_count = -1;
  int term = 0;
  int ctr;
  int lined = 0;
  while((ch = database[i++])!=0)
  {
    if(ch == '\n'||i==1)
    {
      line_count++;
      for(ctr = i;database[ctr]!='|';ctr++);
      wsize = ctr-i+1;
      dbindex[line_count].name = (char*)malloc(wsize*sizeof(char));
      dbindex[line_count].length=0;
      term = 0;
    }
    else if(ch == '|')
    {
      term = 1;
      dbindex[line_count].name[lined]=0;
      lined = 0;
    }
    else
    {
      if(!term)
        dbindex[line_count].name[lined++]=ch;
      else
        dbindex[line_count].length = 10*dbindex[line_count].length+ch-'0';
    }
  }
  free(database);
  return dbindex;
 }
