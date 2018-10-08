/* common.c */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "common.h"

char *program;
char *cursor, peek;
int lineno = 0;

char *get_field(char *term)
{
     char *result, *end;
     int level = 0;

     while (*cursor == ' ')
	  cursor++;
     result = cursor;
     while (*cursor != '\0' &&
	    (level > 0
	     || ! strchr(term, *cursor)
	     || cursor[0] == '-' && isalnum(cursor[1])
	     || cursor[0] == '-' && cursor[1] == '-'
	     || cursor[0] == '(' && cursor[1] != '$')) {
	  if (*cursor == '{')
	       level++;
	  else if (*cursor == '}')
	       level--;
	  cursor++;
     }
     if (*cursor == '\0') {
	  fprintf(stderr, "unexpected end of line on line %d\n", lineno);
	  exit(1);
     }
     peek = *cursor;
     end = cursor-1;
     while (*end == ' ')
	  end--;
     end[1] = '\0';
     cursor++;
     return result;
}

void load_file(void)
{
     static char line[MAXLINE];

     while (fgets(line, MAXLINE, stdin) != NULL) {
	  lineno++;
	  cursor = strchr(line, '{');
	  if (cursor == NULL) {
	       fprintf(stderr, "Missing '{' on line %d\n", lineno);
	       exit(1);
	  }
	  read_record();
     }
}
	
/* lexico -- compare strings, ignoring non-alphanumerics.  Ignore case 
     too unless that's the only difference. */
int lexico(char *s, char *t)
{
     char *a = s, *b = t;
     int withcase = 0;

     for (;;) {
	  while (*a != '\0' && ! isalnum(*a) && *a != ' ') a++;
	  while (*b != '\0' && ! isalnum(*b) && *b != ' ') b++;
	  if (! isalnum(*a) && ! isalnum(*b))
	       break;
	  else if (! isalnum(*a))
	       return -1;
	  else if (! isalnum(*b))
	       return 1;
	  else if (tolower(*a) != tolower(*b))
	       return tolower(*a) - tolower(*b);
	  else if (withcase == 0)
	       withcase = *a - *b;
	  a++; b++;
     }

     if (withcase != 0) return withcase;
     return strcmp(s, t);
}

void usage(void)
{
     fprintf(stderr, "usage: %s [infile] [outfile]\n", program);
     exit(2);
}

void setup_and_read(int argc, char *argv[])
{
     program = argv[0];

     if (argc > 3)
	  usage();

     if (argc >= 2 && freopen(argv[1], "r", stdin) == NULL) {
	  fprintf(stderr, "%s: can't read %s\n", program, argv[1]);
	  exit(1);
     }

     if (argc >= 3 && freopen(argv[2], "w", stdout) == NULL) {
	  fprintf(stderr, "%s: can't write to %s\n", program, argv[2]);
	  exit(1);
     }

     load_file();
}
