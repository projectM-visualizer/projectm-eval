#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>  /* For printf, etc. */
#include <math.h>   /* For pow, used in the grammar. */

#define YYSTYPE PRJM_EEL_STYPE
#define YYLTYPE PRJM_EEL_LTYPE

/* Function type. */
typedef float (func_t)(float);

/* Data type for links in the chain of symbols. */
struct symrec
{
    char* name;  /* name of symbol */
    int type;    /* type of symbol: either VAR or FUN */
    union
    {
        double var;    /* value of a VAR */
        func_t* fun;   /* value of a FUN */
    } value;
    struct symrec* next;  /* link field */
};

typedef struct symrec symrec;

/* The symbol table: a chain of 'struct symrec'. */
//extern symrec* sym_table;

symrec* putsym(char const* name, int sym_type);
symrec* getsym(char const* name);
void prjm_eel_error(char const* s);

struct init
{
    char const* name;
    func_t* fun;
};

#ifdef __cplusplus
}
#endif
