void tolowerstr(char[]);
int bin_search(long, char**, char[],int(*cmp_fn)(int, char**, char*));
int bin_search_struct(long, struct index_word*, char* ,int);
char** extract_keywords(long, char**, char[],int*);
int strcmp_bin(int , char** , char*);
long long get_hash_key(char*);

char* porter_stemmer(char*);
