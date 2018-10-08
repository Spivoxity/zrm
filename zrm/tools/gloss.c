/* gloss.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "common.h"

typedef struct item *item;

typedef struct item {
     char *key;
     char *text;
     int page;
} item_rec;

item item_table[MAXITEMS];
int n_items = 0;

void read_record(void)
{
     item p = malloc(sizeof(item_rec));
     cursor++;
     get_field("[");
     p->key = strdup(get_field("]"));
     p->text = strdup(get_field("}"));
     cursor++;
     p->page = atoi(get_field("}"));
     item_table[n_items++] = p;
}

int item_compare(item *a, item *b)
{
     return lexico((*a)->key, (*b)->key);
}

char buf[MAXLINE];

void make_gloss(void)
{
     int i;
     char *w;

     printf("\\begin{theglossary}\n");
     for (i = 0; i < n_items; i++) {
	  item p = item_table[i];
	  int pos = 0;
	  sprintf(buf, "\\item[%s] %s (p.~%d)", p->key, p->text, p->page);
	  for (w = strtok(buf, " "); w != NULL;
		    w = strtok((char *) NULL, " ")) {
	       int wlen = strlen(w);
	       if (pos + wlen + 1 > MAXOUT) {
		    printf("\n  %s", w);
		    pos = wlen + 2;
	       }
	       else if (pos == 0) {
		    printf("%s", w);
		    pos += wlen;
	       }
	       else {
		    printf(" %s", w);
		    pos += wlen + 1;
	       }
	  }
	  printf("\n");
     }
     printf("\\end{theglossary}\n");
}

typedef int (*compare_fun)(const void *, const void *);

int main(int argc, char *argv[])
{
     setup_and_read(argc, argv);
     qsort(item_table, n_items, sizeof(item), (compare_fun) item_compare);
     make_gloss();
     return 0;
}
