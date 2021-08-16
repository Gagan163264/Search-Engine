struct freq_per_doc
{
  char* orword;
  char* docname;
  int freq;
};

struct index_word
{
  char* word;
  long long hash_key;
  struct freq_per_doc* doc_data;
};

struct docdet
{
  char* name;
  int length;
};

char* importdb_tomem(long *, char *);
char** import_stopdb_tomem(long *, char *);
struct index_word* import_index_tomem(long *, char *);
int export_index(struct index_word*, long, char*);
struct docdet* import_dbindex(long*, char*);
int reversenum(int);
