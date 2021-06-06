#include<stdio.h>
#include<string.h>
#include"search.c"

#define INPUT_LIMIT 50
#define COLUMN_SEPERATOR '|'
#define ROW_SEPERATOR '\0'
#define DATABASE "db.txt"



int main()
{

  int c;                                      //init
  int element_str_counter = 0;
  int line_pos_counter = 0;
  int line_counter = 1;
  int match_notfound_flag = 1;
  char element_str[INPUT_LIMIT];
  char line_str[512];
  memset(element_str, 0, sizeof element_str);
  memset(line_str, 0, sizeof line_str);
  FILE *file = fopen(DATABASE, "r");


  char uin_str[INPUT_LIMIT];                 // get user input
  printf("Search For: ");
  fgets(uin_str, INPUT_LIMIT, stdin);
  printf("Running search for: %s", uin_str);
  uin_str[strcspn(uin_str, "\n")] = 0;

 if (file)
 {
   while(fgets(line_str, 512, file))                             //get line from file
    {
      while((c = line_str[line_pos_counter]) != ROW_SEPERATOR)           // get character from line
      {
        if(c == COLUMN_SEPERATOR && line_str[line_pos_counter+1] == COLUMN_SEPERATOR)     //end element at seperator
        {
          if(!strcmp(element_str, uin_str))
          {
           printf("Match found in line %d\n", line_counter);
           printf("%s", line_str);
           match_notfound_flag = 0;
          }
          memset(element_str, 0, sizeof element_str);
          element_str_counter = 0;
          line_pos_counter++;
        }
        else
        {
        element_str[element_str_counter]=c;
        element_str_counter++;
        }
      line_pos_counter++;
      }
      memset(line_str, 0, sizeof line_str);
      line_pos_counter = 0;
      line_counter++;
    }
 }
 fclose(file);
 if(match_notfound_flag)
  printf("Your search %s did not match any entries\n", uin_str);
 }
