struct freq_per_doc
{
  char* docname;
  int freq;
};

struct index_word
{
  char* word;
  int freq_across_docs;
  struct freq_per_doc* doc_data;
};

char* importdb_tomem(long *, char *);
char** import_stopdb_tomem(long *, char *);
struct index_word* import_index_tomem(long *, char *);
int export_index(struct index_word*, long, char*);
