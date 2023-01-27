#include "Calc.h"

extern "C" {
#include "Compiler.h"
}

/* The mfcalc code assumes that malloc and realloc
   always succeed, and that integer calculations
   never overflow.  Production-quality code should
   not make these assumptions.  */
#include <cstdlib> /* malloc, realloc. */
#include <cstring> /* strlen. */

struct init const funs[] =
    {
        { "atan", atan },
        { "cos",  cos  },
        { "exp",  exp  },
        { "ln",   log  },
        { "sin",  sin  },
        { "sqrt", sqrt },
        { "megabuf", sin },
        { "gmegabuf", sin },
        { 0, 0 },
    };

/* The symbol table: a chain of 'struct symrec'. */
symrec* sym_table{ nullptr };

/* Put functions in table. */
void init_table()
{
    for (int i = 0; funs[i].name; i++)
    {
        symrec *ptr = putsym(funs[i].name, FUN);
        ptr->value.fun = funs[i].fun;
    }
}

symrec* putsym (char const *name, int sym_type)
{
    symrec *res = (symrec *) malloc (sizeof (symrec));
    res->name = strdup (name);
    res->type = sym_type;
    res->value.var = 0; /* Set value to 0 even if fun. */
    res->next = sym_table;
    sym_table = res;
    return res;
}

symrec* getsym(char const *name)
{
    for (symrec *p = sym_table; p; p = p->next)
        if (strcmp (p->name, name) == 0)
            return p;
    return nullptr;
}

/* Called by yyparse on error. */
void prjm_eel_error (char const *s)
{
    fprintf (stderr, "%s\n", s);
}

int main (int argc, char const* argv[])
{
    /* Enable parse traces on option -p. */
    /*
    if (argc == 2 && strcmp(argv[1], "-p") == 0)
        prjm_eel_debug = 1;
        */
    init_table ();
    return prjm_eel_parse();
}
