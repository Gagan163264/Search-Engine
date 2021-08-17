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
  clock_t timer;
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
  timer = clock();
  fgets(raw_uin_str,INPUT_LIMIT,stdin);
  printf("Running search for: %s\n", raw_uin_str);
  raw_uin_str[strcspn(raw_uin_str, "\n")] = 0;
  strcpy(raw_uin_str_cp, raw_uin_str);
  tolowerstr(raw_uin_str_cp);

  int kwsize;
  int i = 0;
  int owordsize;
  int temp;
  char** uin_wtable = extract_keywords(stop_db_size, stop_db_arr, raw_uin_str_cp,&kwsize);

  int* keywords = (int*)malloc(kwsize*sizeof(int));
  int* owordsdocfreq = (int*)calloc(kwsize,sizeof(int));
  int* swordsdocfreq = (int*)calloc(kwsize,sizeof(int));
  int* swordsfreq = (int*)calloc(kwsize,sizeof(int));
  float* oworddoclist = (float*)calloc(size_dbindex,sizeof(float));
  float* sworddoclist = (float*)calloc(size_dbindex,sizeof(float));
  char** stemmed_uin_wtable = (char**)malloc((kwsize+1)*sizeof(char*));
  stemmed_uin_wtable[kwsize]=NULL;

  int odocfreq = 0;
  int odoclen = 0;
  int sdocfreq = 0;
  int sdoclen = 0;
  int rwflag;
  for(i = 0;uin_wtable[i]!= NULL;i++)
  {
    stemmed_uin_wtable[i]=(char*)malloc((strlen(uin_wtable[i])+1)*sizeof(char));
    strcpy(stemmed_uin_wtable[i],uin_wtable[i]);
    porter_stemmer(stemmed_uin_wtable[i]);
    keywords[i]=bin_search_struct(size_index,index,stemmed_uin_wtable[i],-1);
    if(keywords[i]<0)
      continue;
    for(owordsize = 0; index[keywords[i]].doc_data[owordsize].orword != NULL; owordsize++)
    {
      rwflag = 0;
      temp = bin_search_struct_2(size_dbindex,dbindex,index[keywords[i]].doc_data[owordsize].docname);
      if(temp<0)
        return 3;//database error
      if(!strcmp(uin_wtable[i],index[keywords[i]].doc_data[owordsize].orword))
      {
        owordsdocfreq[i]++;
        if(!oworddoclist[temp])
        {
          rwflag = 1;
          odocfreq++;
          odoclen += dbindex[temp].length;
          oworddoclist[temp]=1;
        }
      }
      swordsdocfreq[i]++;
      if(!sworddoclist[temp]&&!rwflag)
      {
        sdocfreq++;
        sdoclen += dbindex[temp].length;
        swordsfreq[i]+= index[keywords[i]].doc_data[owordsize].freq;
        sworddoclist[temp]=1;
      }
    }
  }
  for(int j = 0; j<size_dbindex;j++)
  {
    if(oworddoclist[j])
    {
      //sworddoclist[j] = 0;
      for(i = 0;uin_wtable[i]!=NULL;i++)
      {
        if(keywords[i]>=0)
          for(int iter = 0; index[keywords[i]].doc_data[iter].orword!=NULL;iter++)
            if(!strcmp(index[keywords[i]].doc_data[iter].orword,uin_wtable[i])&&!strcmp(index[keywords[i]].doc_data[iter].docname,dbindex[j].name))
            {
              oworddoclist[j]+=rank(get_idf(odocfreq,owordsdocfreq[i]),index[keywords[i]].doc_data[iter].freq,dbindex[j].length,(odoclen/odocfreq));
              break;
            }
      }
     }
    if(sworddoclist[j])
      for(i = 0;uin_wtable[i]!=NULL;i++)
      {
        sworddoclist[j]+=rank(get_idf(sdocfreq,swordsdocfreq[i]),swordsfreq[i],dbindex[j].length,(sdoclen/sdocfreq));
        break;
      }
  }
  int res;
  int loopctr = 1;
  int* filematch = (int*)malloc(size_dbindex*sizeof(int));
  memset(filematch,-1,size_dbindex*sizeof(int));
  while((res=pop_max(oworddoclist,sworddoclist,size_dbindex))>=0)
  {
    if(loopctr==1)
      printf("Best match: %s\n", dbindex[res].name);
    else
      printf("%d%s match: %s\n",loopctr,(loopctr==1)?"st":((loopctr==2)?"nd":((loopctr==3)?"rd":"th")),dbindex[res].name);
    filematch[loopctr-1]=res;
    loopctr++;
  }
  if(loopctr==1)
    printf("Your search '%s' did not match any known documents\n", raw_uin_str_cp);
  else
  {
    printf("\nTo open any of the matched documents(should have atom installed), type their position number, to end press enter: ");
    char ch = getchar();
    int chr = ch-'0';
    if(ch != '\n')
      getchar();
    if(ch!='\n')
    {
      while(ch<49||ch>57||chr>=(loopctr-1))
      {
        printf("\nYou have entered invalid index number, try again:");
        ch = getchar();
        if(ch == '\n')
          break;
        getchar();
        chr = ch-'0'-1;
      }
      if(ch != '\n')
      {
      char* openpath = (char*)malloc((strlen(dbindex[filematch[chr]].name)+12)*sizeof(char));
      strcpy(openpath,"atom source/");
      strcat(openpath,dbindex[filematch[chr]].name);
      system(openpath);
      }
    }
  }
  timer = clock() - timer;
  double time = ((double)timer)/(CLOCKS_PER_SEC/1000000);

  printf("\nSimple search took %.1lf microseconds to return %d results\n", time, loopctr-1);

}
