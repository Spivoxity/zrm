/* common.h */

#define MAXLINE 1024	      /* Max line length (big for glossary) */
#define HASH 256	      /* Size of hash table */
#define MAXITEMS 1024	      /* Max no of main entries */
#define MAXSUBS 128	      /* Max no of subentries per main entry */
#define MAXPAGES 32	      /* Max pages per symdex entry */
#define MAXOUT 72	      /* Max length of output line (gloss) */
#define MINCORE 1024	      /* Min core left before panicking */

typedef enum { FALSE, TRUE } bool;

extern char *cursor, peek;
extern int lineno;

char *get_field(char *term);
int lexico(char *a, char *b);
void setup_and_read(int argc, char *argv[]);

/* Each program provides ... */
void read_record(void);
