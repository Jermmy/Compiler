#ifndef REGEX_H_
#define REGEX_H_

#include <iostream>
#include <vector>
#include <set>

using namespace std;

enum
{
    LeftmostBiased = 0,
    LeftmostLongest = 1,
};

enum
{
    RepeatMinimal = 0,
    RepeatLikePerl = 1,
};

enum
{
    NSUB = 10
};

typedef struct Sub Sub;
struct Sub
{
    const char *sp;
    const char *ep;
};

enum {
    CharIncluded = 0,
    CharExcluded = 1,
};

enum {
    NCHAR = 128,
};
typedef struct Range Range;
struct Range {
    int type;
    char flag[NCHAR];
};

typedef union Data Data;
union Data
{
    int val;
    Range range;
};

enum
{
    Char = 1,
    Any = 2,
    Split = 3,
    LParen = 4,
    RParen = 5,
    Match = 6,
    CharClass = 7,
};
typedef struct State State;
typedef struct Thread Thread;
struct State
{
    int op;
    Data data;
    State *out;
    State *out1;
    int id;
    int lastlist;
    int visits;
    Thread *lastthread;
};

struct Thread
{
    State *state;
    Sub match[NSUB];
};

typedef struct List List;
struct List
{
    Thread *t;
    int n;
};

/* Allocate and initialize State */
State*
state(int op, int val, State *out, State *out1);

/* Allocate and initialize CharClass State */
State*
ccstate(int op, Range range, State *out, State *out1);

typedef struct Frag Frag;
typedef union Ptrlist Ptrlist;
struct Frag
{
    State *start;
    Ptrlist *out;
};

/* Initialize Frag struct. */
Frag
frag(State *start, Ptrlist *out);

/*
 * Since the out pointers in the list are always
 * uninitialized, we use the pointers themselves
 * as storage for the Ptrlists.
 */
union Ptrlist
{
    Ptrlist *next;
    State *s;
};

/* Create singleton list containing just outp. */
Ptrlist*
list1(State **outp);

/* Patch the list of states at out to point to start. */
void
patch(Ptrlist *l, State *s);

/* Join the two lists l1 and l2, returning the combination. */
Ptrlist*
append(Ptrlist *l1, Ptrlist *l2);

void yyerror(const char*);
int yylex(void);

Frag
paren(Frag f, int n);

typedef union YYSTYPE YYSTYPE;
union YYSTYPE {
    Frag frag;
    int c;
    int nparen;
    Range range;
};

void dumplist(List*);

enum
{
    EOL = 0,
    CHAR = 257,
    CHARCLASS = 258,
};

int
yylex(void);

void
move();

int
matchtoken(int t);

Frag single();
Frag repeat();
Frag concat();
Frag alt();
void line();

void
line();

Frag
alt();

Frag
concat();

Frag
repeat();

Frag
single();

void
yyparse();

void
yyerror(const char *s);

void
printmatch(Sub *m, int n);

void
dumplist(List *l);

/*
 * Is match a longer than match b?
 * If so, return 1; if not, 0.
 */
int
longer(Sub *a, Sub *b);

/*
 * Add s to l, following unlabeled arrows.
 * Next character to read is p.
 */
void
addstate(List *l, State *s, Sub *m, const char *p);

/*
 * Step the NFA from the states in clist
 * past the character c,
 * to create next NFA state set nlist.
 * Record best match so far in match.
 */
void
step(List *clist, int c, const char *p, List *nlist, Sub *match);

/* Compute initial thread list */
List*
startlist(State *start, const char *p, List *l);

int
match(State *start, const char *p, Sub *m);

void
dump(State *s);

static set<State *> freenodes;
void freenfa(State *state);

vector<vector<int> >
findall(const char *regex, const char *content);
#endif
