#include "TreeVariables.h"

#include <stdlib.h>
#include <string.h>

static prjm_eel_variable_entry_t* find_variable_entry(prjm_eel_compiler_context_t* cctx,
                                                      const char* name)
{
    /* Find existing variable */
    prjm_eel_variable_entry_t* var = cctx->variables.first;
    while (var)
    {
        if (strcasecmp(var->variable->name, name) == 0)
        {
            break;
        }
        var = var->next;
    }

    return var;
}

prjm_eel_variable_def_t* prjm_eel_register_variable(prjm_eel_compiler_context_t* cctx,
                                                    const char* name,
                                                    PRJM_F* value_ptr)
{
    prjm_eel_variable_entry_t* var = find_variable_entry(cctx, name);

    /* Create if it doesn't exist */
    if (!var)
    {
        var = malloc(sizeof(prjm_eel_variable_entry_t));
        var->variable = calloc(1, sizeof(prjm_eel_variable_def_t));
        var->variable->name = strdup(name);
        var->variable->value = .0f;
        var->variable->value_ptr = &var->variable->value;
        var->next = cctx->variables.first;
        cctx->variables.first = var;
    }

    if (value_ptr)
    {
        var->variable->value_ptr = value_ptr;
    }

    return var->variable;
}

void prjm_eel_unregister_variable(prjm_eel_compiler_context_t* cctx,
                                  const char* name)
{
    prjm_eel_variable_entry_t* var = find_variable_entry(cctx, name);

    if (var)
    {
        var->variable->value_ptr = &var->variable->value;
    }
}