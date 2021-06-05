#include<stdio.h>
#include<string.h>
#define INPUT_LIMIT 50

int main()
{

  char uin_str[INPUT_LIMIT];                 // get user input
  printf("Search For: ");
  fgets(uin_str, INPUT_LIMIT, stdin);
  printf("Running search for: %s", uin_str);
  uin_str[strcspn(uin_str, "\n")] = 0;

 int c;
 int element_str_counter = 0;
 FILE *file;
 char element_str[INPUT_LIMIT];
 file = fopen("db.txt", "r");
 if (file)
 {
   while ((c = getc(file)) != EOF)
   {
     /*if(c == '\n' || c == '|')
     {
       printf("%s\n", element_str);
       //if(!strcmp(element_str, uin_str))
        //printf("Match found");
       memset(element_str, 0, sizeof element_str);
     }
     element_str[element_str_counter]=c;
     */
     printf("%c", c);
   }
   fclose(file);
 }




}
