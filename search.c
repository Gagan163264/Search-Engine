#include<stdio.h>
#include<string.h>
#include<ctype.h>

#define WORD_SEPERATOR '|'
#define SENTENCE_SEPERATOR ';' || c == ' '

void tolowerstr(char arr[])
{
  int i = 0;
  while(arr[i] != 0)
  {
    arr[i] = tolower(arr[i]);
    i++;
  }
}

int simple_search(char uin_str[], char database[], long database_size)
{
  int i = 0;
  int sentence_count = 1;
  int file_line = 1;
  int word_pos_counter = 0;
  int sentence_pos_counter = 0;
  int match_notfound_flag = 1;
  int quote_flag = 0;
  int hit_counter = 0;
  int total_hit_counter = 0;
  char sentence_str[512];
  char word_str[50];
  memset(word_str, 0, sizeof word_str);
  memset(sentence_str, 0, sizeof sentence_str);
  char c;

  tolowerstr(uin_str);

  while((c = database[i++]) != 0)
  {
    if(c == WORD_SEPERATOR || c == SENTENCE_SEPERATOR || c == '\n' || c ==',' || c == '\'' || c == ' ')
    {
      if(!strcmp(word_str, uin_str))
        {
          hit_counter++;
          total_hit_counter++;
        }
      memset(word_str, 0, sizeof word_str);
      word_pos_counter = 0;
      if(c =='\n')
        file_line++;
      if(c == WORD_SEPERATOR || c == '\'' || c == ',')
        sentence_str[sentence_pos_counter++]=c;
      if(c == SENTENCE_SEPERATOR)
      {
        sentence_str[sentence_pos_counter] = '.';
        if(i>1 && (database[i] == SENTENCE_SEPERATOR || database[i-2] == SENTENCE_SEPERATOR))
        {
          word_str[word_pos_counter++]=tolower(c);
          sentence_str[sentence_pos_counter++]=c;
          continue;
        }
        if(hit_counter)
        {
          printf("\n%d match", hit_counter);
          if(hit_counter > 1)
            printf("es");
          //printf(" found in sentence %d in line %d:\n", sentence_count, file_line);  //to be used in text file
          printf(" found in line %d:\n",file_line);                                  //to be used in databases(optional)
          printf("%s\n", sentence_str);
          match_notfound_flag = 0;
        }
        memset(sentence_str, 0, sizeof sentence_str);
        sentence_count++;
        sentence_pos_counter=0;
        hit_counter = 0;
      }
    }
    else
    {
      word_str[word_pos_counter++]=tolower(c);
      sentence_str[sentence_pos_counter++]=c;
    }
  }
  if(match_notfound_flag)
    printf("Your search %s did not match any entries\n", uin_str);
  return total_hit_counter;
}
