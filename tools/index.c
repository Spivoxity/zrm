/* index.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "common.h"

typedef struct token *token;
typedef struct item *item;
typedef struct pageref *pageref;

typedef struct pageref {
     char *number;
     bool hilite;
     char flag;
     pageref next;
} page_rec;

typedef struct item {
     token main;
     token sub;
     pageref pages, last_page;
     char *symbol;
     char *cross_ref;
     item next;
} item_rec;

typedef struct token {
     char *text;
     item entry;
     token link;
} token_rec;

token hash_table[HASH];
token item_table[MAXITEMS];
int n_items = 0;
int warnings = 0;

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
     p->entry = NULL;
     p->link = hash_table[h];
     hash_table[h] = p;
     return p;
}

struct {
     token main;
     token sub;
     char *symbol;
     char flag;
     char *see_also;
     char *page;
     bool star;
} record;

void parse_record(void)
{
     cursor++;
     record.main = lookup(get_field(":(*+-!>}"));
     if (peek == ':')
	  record.sub = lookup(get_field("(*+-!>}"));
     else
	  record.sub = NULL;
     if (peek != '(')
	  record.symbol = NULL;
     else {
	  record.symbol = cursor;
	  do
	       cursor++;
	  while (cursor[-1] != '$' || *cursor != ')');
	  *cursor++ = '\0';
	  get_field("*+-!>}");
     }
     record.star = (peek == '*');
     if (record.star)
	  get_field("+-!>}");
     if (strchr("+-!>", peek)) {
	  record.flag = peek;
	  record.see_also = get_field("}");
     }
     else
	  record.flag = ' ';
     cursor++;
     record.page = get_field("}");
}

void add_info(item p)
{
     pageref pp;

     if (record.symbol != NULL)
	  p->symbol = strdup(record.symbol);

     switch (record.flag) {
     case '!':
	  break;

     case '>':
	  if (p->cross_ref != NULL)
	       fprintf(stderr, "warning: only one cross-ref per item\n");
	  else
	       p->cross_ref = strdup(record.see_also);
	  break;

     default:
	  pp = (pageref) malloc(sizeof(page_rec));
	  pp->number = strdup(record.page);
	  pp->hilite = record.star;
	  pp->flag = record.flag;
	  pp->next = NULL;
	  if (p->pages == NULL)
	       p->pages = pp;
	  else
	       p->last_page->next = pp;
	  p->last_page = pp;
	  break;
     }
}

item new_item(token main, token sub)
{
     item p = (item) malloc(sizeof(item_rec));
     p->main = main;
     p->sub = sub;
     p->pages = p->last_page = NULL;
     p->symbol = p->cross_ref = NULL;
     p->next = NULL;
     return p;
}

void read_record(void)
{
     item p;

     parse_record();

     if (record.main->entry == NULL) {
	  record.main->entry = new_item(record.main, NULL);
	  item_table[n_items++] = record.main;
     }
     p = record.main->entry; 
     while (p != NULL && p->sub != record.sub) 
	  p = p->next;
     if (p == NULL) {
	  p = new_item(record.main, record.sub);
	  p->next = record.main->entry->next;
	  record.main->entry->next = p;
     }
     add_info(p);
}

int item_compare(token *a, token *b)
{
     return lexico((*a)->text, (*b)->text);
}

int sub_compare(item *a, item *b)
{
     return lexico((*a)->sub->text, (*b)->sub->text);
}

void print_range(char *start, char *end)
{
     int j = 0;

     /* If numbers have same no of digits, merge common prefix */
     /* ... but print 120--21, 100--121, not 120--1, 100--21 */
     /* ... and print 214--16, not 214--6 */
     if (strlen(start) == strlen(end))
	  while (start[j] == end[j]) j++;
     if (j > 0 && start[j+1] == '\0' && start[j-1] == '1') j--;
     if (strspn(&start[j], "0") == strlen(&start[j]))
	  while (start[j] == '0') j--;
     printf(", %s--%s", start, &end[j]);
}

void print_pages(pageref pages)
{
     pageref p, q;
     char *num;

     for (q = pages; q != NULL; q = q->next) {
	  p = q;
	  num = q->number;
	  for (;;) {
	       if (q->flag == '+') {
		    while (q->next != NULL && q->flag != '-')
			 q = q->next;
		    if (q->flag != '-') {
			 if (warnings++ == 0)
			      fprintf(stderr, "Warning: unclosed range\n");
			 printf(", %s--??", p->number);
			 return;
		    }
		    num = q->number;
	       }
	       if (q->next == NULL
		   || strcmp(num, q->next->number) != 0)
		    break;
	       q = q->next;
	  }
	  if (strcmp(p->number, q->number) == 0)
	       /* Put a tie before the last number. */
	       printf(",%c%s", (q->next == NULL ? '~' : ' '), p->number);
	  else
	       print_range(p->number, q->number);
     }
}

void textout(char *tag, char *txt)
{
     char *s;

     printf("%s ", tag);
     for (s = txt; *s != '\0'; s++) {
	  if (*s != '|')
	       putchar(*s);
	  else {
	       printf("\\verb:");
	       for (s++; *s != '\0' && *s != '|'; s++)
		    putchar(*s);
	       putchar(':');
	       if (*s == '\0') s--;
	  }
     }
}

void do_entry(item p, bool run_on)
{
     if (p->sub == NULL)
	  textout("\\item", p->main->text);
     else if (run_on) {
	  textout("\\item", p->main->text);
	  textout(",", p->sub->text);
     }
     else
	  textout("  \\subitem", p->sub->text);
     if (p->symbol != NULL)
	  printf(" (%s)", p->symbol);
     print_pages(p->pages);
     if (p->cross_ref != NULL)
	  textout((p->pages == NULL ? ", {\\em see\\/}" 
		   : " {\\em see also\\/}"), p->cross_ref);
     printf("\n");
}

typedef int (*compare_fun)(const void *, const void *);

void do_item(item main)
{
     char *xref = NULL;
     static item sub_item[MAXSUBS];
     int n_subs = 0;
     bool run_on;
     item s;
     int i;

     for (s = main->next; s != NULL; s = s->next)
	  sub_item[n_subs++] = s;
     qsort(sub_item, n_subs, sizeof(item), (compare_fun) sub_compare);

     if (n_subs > 0) {
	  xref = main->cross_ref;
	  main->cross_ref = NULL;
     }
     run_on = (main->pages == NULL && main->symbol == NULL
	       && n_subs == 1 && xref == NULL);

     if (! run_on)
	  do_entry(main, FALSE);
     for (i = 0; i < n_subs; i++)
	  do_entry(sub_item[i], run_on);
     if (xref != NULL) {
	  textout("  \\subitem {\\em see also\\/}", xref);
	  printf("\n");
     }
}

/* initial -- the first letter of a string in lower-case, '!' if none */
char initial(char *s)
{
     while (*s != '\0' && *s != ' ' && !isalpha(*s)) s++;
     return (isalpha(*s) ? tolower(*s) : '!');
}

void make_index(void)
{
     int i;
     char init = '&', new_init;

     printf("\\begin{theindex}\n");
     for (i = 0; i < n_items; i++) {
	  new_init = initial(item_table[i]->text);
	  if (new_init != init && init != '&')
	       printf("\\indexspace %% %c\n", new_init);
	  init = new_init;

	  do_item(item_table[i]->entry);
     }
     printf("\\typeout{Remember to balance the last page of the index.}\n");
     printf("\\end{theindex}\n");
}

int main(int argc, char *argv[])
{
     setup_and_read(argc, argv);
     qsort(item_table, n_items, sizeof(token), (compare_fun) item_compare);
     make_index();
     return 0;
}
