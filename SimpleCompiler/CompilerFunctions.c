#include "CompilerFunctions.h"

#include <string.h>

/* Called by yyparse on error. */
void prjm_eel_error(PRJM_EEL_LTYPE* loc, prjm_eel_compiler_context_t* cctx, yyscan_t yyscanner, char const * s)
{
    fprintf (stderr, "%s\n", s);
}

bool prjm_eel_compiler_name_is_function(prjm_eel_compiler_context_t* cctx, const char* name)
{
    prjm_eel_function_entry_t* entry = cctx->functions.first;
    while(entry)
    {
        if (strcmp(entry->function->name, name) == 0)
        {
            return true;
        }

        entry = entry->next;
    }

    return false;
}