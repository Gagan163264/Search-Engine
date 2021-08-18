#include <dirent.h>
#include <ctype.h>
#include "include.h"

int main(void)
{
  FILE *fptr;  //declarations
  int check_txt, path_append;
  char c;
  char* dot;
  struct dirent *de;
  char fpath[100]="source/";
  char index_template_path[] = "SE-db/index_template.txt";
  char index_path[]="SE-db/index.txt";
  char stop_db_path[]="SE-db/stop-words.txt";
  char output_dump[]="SE-db/out.txt";
  char dcindx[]="SE-db/document-index.txt";

  FILE* outfil = fopen(output_dump, "w");
  FILE* doc_index = fopen(dcindx, "w");
  struct docdet* docindx = (struct docdet*)malloc(sizeof(struct docdet));
  docindx[0].name=NULL;
  int docdsize =0;
  long stop_size;
  char** stop_db_arr = import_stopdb_tomem(&stop_size, stop_db_path);
  DIR *dr = opendir("source");
  if (dr == NULL)  // opendir returns NULL if couldn't open directory
  {
      printf("Could not open current directory\n" );
      return 2;
  }

  long size_index;             //import index structure
  struct index_word* index = import_index_tomem(&size_index, index_template_path);
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
  int index_pos;
  int substructindex;
  int sub_index_pos;
  int idx_size;
  int numpr;
  int wrdcount;
  int strcp;
  int flipflag = 0;
  int flipnumber,flipnum;

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
    wrdcount=0;
    while(rword != NULL )
    {
      endword =0;
      beginword = 0;
      nlcount = 0;
      while((ch = rword[endword]))
      {
        if(ch=='+'||ch=='<'||ch=='>'||ch=='^'||ch=='/'||ch=='(')//https://docs.oracle.com/cd/E29584_01/webhelp/mdex_basicDev/src/cbdv_searchchar_indexing_non-alphanumeric_characters.html
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
        while((raw_word[beginword])&&(!isalpha(raw_word[beginword])))
          beginword++;
        if(beginword==endword)
          break;
        endword--;
        while(((endword)>=0)&&(!isalpha(raw_word[endword])))
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
              numpr=0;
              word = (char*)malloc((endvar-startvar+2)*sizeof(char));
              for(int i = 0;i<=endvar;i++)
              {
                word[i]=tolower(raw_word[startvar+i]);
                numpr = numpr+isalpha(word[i]);
              }
              word[endvar-startvar+1]=0;
            }
            oword = (char*)malloc((endvar-startvar+2)*sizeof(char));
            strcpy(oword, word);
            if(word[0]&&numpr)//final word
            {
              puts(word);
              wrdcount++;
              if(bin_search(stop_size,stop_db_arr,word,strcmp_bin)>=0)
              //{//enable for stopword isolation
                porter_stemmer(word);
              index_pos=bin_search_struct(size_index,index, word,-1);
              if(index_pos>=0)
              {
                substructindex=0;
                while(index[index_pos].doc_data[substructindex].docname!=NULL)
                  substructindex++;
                sub_index_pos=bin_search_struct(substructindex,index,oword,index_pos);
                if(sub_index_pos>=0)
                {
                  flipflag = 0;
                  flipnum = 0;
                  flipnumber = 0;
                  strcp = strcmp(de->d_name,index[index_pos].doc_data[sub_index_pos].docname);
                  if(strcp>0)
                    flipnum=1;
                  while(strcp)
                  {
                    if(strcp>0)
                    {
                      sub_index_pos++;
                      flipnumber = 1;
                    }
                    else
                    {
                      sub_index_pos--;
                      flipnumber=0;
                    }
                    if(sub_index_pos<0||(sub_index_pos>0&&index[index_pos].doc_data[sub_index_pos].orword==0)||strcmp(index[index_pos].doc_data[sub_index_pos].orword, oword))
                    {
                      if(!flipnumber)
                        sub_index_pos++;
                      flipnum = !flipnumber;
                    }
                    //printf("%s||%s||%d*%d\n",de->d_name,index[index_pos].doc_data[sub_index_pos].docname,strcp, sub_index_pos);
                    if(flipnum!=flipnumber)
                    {
                      flipflag = 1;
                      index[index_pos].doc_data=(struct freq_per_doc*)realloc(index[index_pos].doc_data,(substructindex+2)*sizeof(struct freq_per_doc));
                      while(substructindex>=sub_index_pos)
                      {
                        index[index_pos].doc_data[substructindex+1]=index[index_pos].doc_data[substructindex];
                        substructindex--;
                      }
                      substructindex++;
                      printf("Updating document list to add '%s' for existing word '%s'(unstemmed '%s')\n\n", de->d_name, word,oword);
                      fprintf(outfil,"Updating document list to add '%s' for existing word '%s'(unstemmed '%s')\n", de->d_name, word,oword);
                      index[index_pos].doc_data[substructindex].orword=(char*)malloc((endvar-startvar+2)*sizeof(char));
                      index[index_pos].doc_data[substructindex].docname=(char*)malloc(check_txt*sizeof(char));
                      strcpy(index[index_pos].doc_data[substructindex].orword, oword);
                      strcpy(index[index_pos].doc_data[substructindex].docname, de->d_name);
                      index[index_pos].doc_data[substructindex].freq=1;

                      break;
                    }
                    strcp = strcmp(de->d_name,index[index_pos].doc_data[sub_index_pos].docname);
                    flipnum = flipnumber;
                  }
                  if(!flipflag)
                  {
                    printf("Found word '%s'(unstemmed '%s') in document '%s' at %dth position in index for the %d%s time\n\n", word, oword,de->d_name, index_pos,index[index_pos].doc_data[sub_index_pos].freq,(index[index_pos].doc_data[sub_index_pos].freq==1)?"st":((index[index_pos].doc_data[sub_index_pos].freq==2)?"nd":((index[index_pos].doc_data[sub_index_pos].freq==3)?"rd":"th")));
                    fprintf(outfil, "Found word '%s'(unstemmed '%s') in document '%s' at %dth position in index for the %d%s time\n", word, oword,de->d_name, index_pos,index[index_pos].doc_data[sub_index_pos].freq,(index[index_pos].doc_data[sub_index_pos].freq==1)?"st":((index[index_pos].doc_data[sub_index_pos].freq==2)?"nd":((index[index_pos].doc_data[sub_index_pos].freq==3)?"rd":"th")));
                    index[index_pos].doc_data[sub_index_pos].freq++;
                  }
                }
                else
                {
                  printf("Updating document list to add '%s' for word '%s'(unstemmed '%s')\n\n", de->d_name, word,oword);
                  fprintf(outfil,"Updating document list to add '%s' for word '%s'(unstemmed '%s')\n", de->d_name, word,oword);
                  index[index_pos].doc_data=(struct freq_per_doc*)realloc(index[index_pos].doc_data,(substructindex+2)*sizeof(struct freq_per_doc));
                  while(substructindex>=((-sub_index_pos)-1))
                  {
                    index[index_pos].doc_data[substructindex+1]=index[index_pos].doc_data[substructindex];
                    substructindex--;
                  }
                  substructindex++;
                  index[index_pos].doc_data[substructindex].orword=(char*)malloc((endvar-startvar+2)*sizeof(char));
                  index[index_pos].doc_data[substructindex].docname=(char*)malloc(check_txt*sizeof(char));
                  strcpy(index[index_pos].doc_data[substructindex].orword, oword);
                  strcpy(index[index_pos].doc_data[substructindex].docname, de->d_name);
                  index[index_pos].doc_data[substructindex].freq=1;
                }
              }
              else
              {
                 idx_size=size_index;
                  index=(struct index_word*)realloc(index,(size_index+2)*sizeof(struct index_word));
                  size_index++;
                  while((idx_size)>=((-index_pos)-1))
                  {
                    index[idx_size+1]=index[idx_size];
                    idx_size--;
                  }
                  idx_size++;
                  printf("Updating index, adding '%s'(hashkey-%lld)(unstemmed '%s') in document '%s' at position %d\n\n",word,index[idx_size].hash_key, oword, de->d_name, -index_pos-1);
                  fprintf(outfil,"Updating index, adding '%s'(hashkey-%lld)(unstemmed '%s') in document '%s' at position %d\n",word,index[idx_size].hash_key, oword, de->d_name, -index_pos-1);
                  index[idx_size].word=(char*)malloc((endvar-startvar+2)*sizeof(char));
                  strcpy(index[idx_size].word, word);
                  index[idx_size].hash_key=get_hash_key(word);
                  index[idx_size].doc_data=(struct freq_per_doc*)calloc(2,sizeof(struct freq_per_doc));
                  index[idx_size].doc_data[0].orword=(char*)malloc((endvar-startvar+2)*sizeof(char));
                  strcpy(index[idx_size].doc_data[0].orword, oword);
                  index[idx_size].doc_data[0].docname=(char*)malloc(check_txt*sizeof(char));
                  strcpy(index[idx_size].doc_data[0].docname, de->d_name);
                  index[idx_size].doc_data[1].docname=NULL;
                  index[idx_size].doc_data[0].freq=1;
              }
            //}//enable for stop word isolation
          }
            free(word);
            free(oword);
            contrindex--;
          }
          hypcount--;
        }
        free(raw_word);
      }
      rword = strtok(NULL, sep);
    }
    docdsize++;
    sub_index_pos = bin_search_struct_2(docdsize,docindx,de->d_name);//sort for document document
    if(sub_index_pos>=0)
    {
      printf("Double file error");
      return 1;
    }
    docindx = (struct docdet*)realloc(docindx,(docdsize+1)*sizeof(struct docdet));
    substructindex = docdsize-1;
    while(substructindex>=((-sub_index_pos)-1))
    {
      docindx[substructindex+1]=docindx[substructindex];
      substructindex--;
    }
    substructindex++;
    fprintf(outfil,"Document %s of word-length %d completed\n",docindx[substructindex].name,wrdcount);
    printf("Document %s of word-length %d completed\n",docindx[substructindex].name,wrdcount);
    docindx[substructindex].name=(char*)malloc(check_txt*sizeof(char));
    strcpy(docindx[substructindex].name,de->d_name);
    docindx[substructindex].length = wrdcount;
    free(fle);
  }
  printf("Search completed, index of %ld words created\n", size_index);
  fprintf(outfil, "Search completed, index of %ld words created\n", size_index);
  export_index(index, size_index, index_path);
  for(int i = 0;docindx[i].name!=NULL;i++)
    fprintf(doc_index, "%s|%d\n",docindx[i].name,docindx[i].length);
  free(index);
  closedir(dr);
  fclose(outfil);
  fclose(doc_index);
  return 0;
}
