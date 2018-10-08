/* symdex.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"

typedef struct token *token;

typedef struct token {
     char *text;
     int n_pages;
     int page[MAXPAGES];
     token link;
} token_rec;

token hash_table[HASH];
token item_table[MAXITEMS];
int n_items = 0;

token lookup(char *text)
{
     char *s;
     int h = 0;
     token p;

     for (s = text; *s != 0; s++)
	  h += *s;
     h %= HASH;

     for (p = hash_table[h]; p != NULL; p = p->link)
	  if (strcmp(text, p->text) == 0)
	       return p;

     p = (token) malloc(sizeof(token_rec));
     p->text = strdup(text);
     p->n_pages = 0;
     p->link = hash_table[h];
     hash_table[h] = p;
     item_table[n_items++] = p;
     return p;
}

void read_record(void)
{
     token key;
     int page;

     cursor++;
     key = lookup(get_field("}"));
     cursor++;
     page = atoi(get_field("}"));
     key->page[key->n_pages++] = page;
}
	
void make_symdex(void)
{
     int i, j;

     printf("\\begin{thesymdex}\n");
     for (i = 0; i < n_items; i++) {
	  token p = item_table[i];
	  printf("%-30s & %d", p->text, p->page[0]);
	  for (j = 1; j < p->n_pages; j++)
	       printf(", %d", p->page[j]);
	  printf(" \\\\\n");
     }
     printf("\\end{thesymdex}\n");
}

int main(int argc, char *argv[])
{
     setup_and_read(argc, argv);
     make_symdex();
     return 0;
}
