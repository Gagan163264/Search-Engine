#include<stdio.h>
#include<stdlib.h>
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
  char dcindx[]="SE-db/document-index.txt";
  char index_path[]="SE-db/index.txt";
  char dbindex_path[] = "SE-db/document-index.txt";
  char** stop_db_arr=import_stopdb_tomem(&stop_db_size,stop_db_path);
  long size_index;            //import index structure
  struct index_word* index = import_index_tomem(&size_index, index_path);
  long size_dbindex;
  struct docdet* dbindex = import_dbindex(&size_dbindex,dbindex_path);
  char raw_uin_str[INPUT_LIMIT];
  char raw_uin_str_cp[INPUT_LIMIT];
  printf("Search for: ");
  fgets(raw_uin_str,INPUT_LIMIT,stdin);
  raw_uin_str[strcspn(raw_uin_str, "\n")] = 0;
  strcpy(raw_uin_str_cp, raw_uin_str);
  tolowerstr(raw_uin_str_cp);
  puts(raw_uin_str_cp);

  int kwsize;
  int i = 0;
  int owordsize;
  int owordindx;
  int temp;
  char** uin_wtable = extract_keywords(stop_db_size, stop_db_arr, raw_uin_str_cp,&kwsize);

  int* keywords = (int*)malloc(kwsize*sizeof(int));
  int* owordsdocfreq = (int*)calloc(kwsize,sizeof(int));
  float* oworddoclist = (float*)calloc(size_dbindex,sizeof(float));
  char** stemmed_uin_wtable = (char**)malloc((kwsize+1)*sizeof(char*));
  stemmed_uin_wtable[kwsize]=NULL;

  int odocfreq = 0;
  int odoclen = 0;
  int idfc;
  int odocindex;

  for(i = 0;uin_wtable[i]!= NULL;i++)
  {
    odocindex = 0;
    idfc = 0;
    stemmed_uin_wtable[i]=(char*)malloc((strlen(uin_wtable[i])+1)*sizeof(char));
    strcpy(stemmed_uin_wtable[i],uin_wtable[i]);
    porter_stemmer(stemmed_uin_wtable[i]);
    keywords[i]=bin_search_struct(size_index,index,stemmed_uin_wtable[i],-1);
    if(keywords[i]<0)
      continue;
    owordindx = 0;
    for(owordsize = 0; index[keywords[i]].doc_data[owordsize].orword != NULL; owordsize++)
      if(!strcmp(uin_wtable[i],index[keywords[i]].doc_data[owordsize].orword))
      {
        temp = bin_search_struct_2(size_dbindex,dbindex,index[keywords[i]].doc_data[owordsize].docname);
        if(temp<0)
          return 3;//database error
        owordsdocfreq[i]++;
        if(!oworddoclist[temp])
        {
          odocfreq++;
          odoclen += dbindex[temp].length;
          oworddoclist[temp]=1;
        }
      }
  }
  for(int j = 0; j<size_dbindex;j++)
  {
    if(oworddoclist[j])
      for(i = 0;uin_wtable[i]!=NULL;i++)
      {
        for(int iter = 0; index[keywords[i]].doc_data[iter].orword!=NULL;iter++)
          if(!strcmp(index[keywords[i]].doc_data[iter].orword,uin_wtable[i])&&!strcmp(index[keywords[i]].doc_data[iter].docname,dbindex[j].name))
          {
            oworddoclist[j]+=rank(get_idf(odocfreq,owordsdocfreq[i]),index[keywords[i]].doc_data[iter].freq,dbindex[j].length,(odoclen/odocfreq));
            break;
          }
      }
      printf("Rank of %s is %f\n", dbindex[j].name, oworddoclist[j]);
  }


  for(i = 0; stemmed_uin_wtable[i]!=NULL;i++)
  {
    puts(uin_wtable[i]);
    puts(stemmed_uin_wtable[i]);
    printf("%d\n", keywords[i]);
  }

}
