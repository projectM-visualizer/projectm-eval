#include "CompilerFunctions.h"

#include "TreeFunctions.h"
#include "TreeVariables.h"

#include <string.h>
#include <stdlib.h>

/* Called by yyparse on error. */
void prjm_eel_error(PRJM_EEL_LTYPE* loc, prjm_eel_compiler_context_t* cctx, yyscan_t yyscanner, char const* s)
{
    fprintf(stderr, "[Parser] ERROR: %s\n", s);
}

void prjm_eel_compiler_destroy_arglist(prjm_eel_compiler_arg_list_t* arglist)
{
    if (!arglist)
    {
        return;
    }

    prjm_eel_compiler_arg_node_t* arg = arglist->begin;
    while (arg)
    {
        prjm_eel_compiler_arg_node_t* free_arg = arg;
        arg = arg->next;
        if (free_arg->node)
        {
            if (free_arg->node->tree_node)
            {
                prjm_eel_destroy_exptreenode(free_arg->node->tree_node);
            }
            free(free_arg->node);
        }
        free(free_arg);
    }

    free(arglist);
}

void prjm_eel_compiler_destroy_node(prjm_eel_compiler_node_t* node)
{
    if (node->tree_node)
    {
        prjm_eel_destroy_exptreenode(node->tree_node);
    }
    free(node);
}

bool prjm_eel_compiler_name_is_function(prjm_eel_compiler_context_t* cctx, const char* name)
{
    prjm_eel_function_list_item_t* entry = cctx->functions.first;
    while (entry)
    {
        if (strcmp(entry->function->name, name) == 0)
        {
            return true;
        }

        entry = entry->next;
    }

    return false;
}

prjm_eel_function_def_t* prjm_eel_compiler_get_function(prjm_eel_compiler_context_t* cctx, const char* name)
{
    prjm_eel_function_list_item_t* func = cctx->functions.first;
    while (func)
    {
        if (strcasecmp(func->function->name, name) == 0)
        {
            return func->function;
        }

        func = func->next;
    }

    return NULL;
}

prjm_eel_compiler_arg_list_t* prjm_eel_compiler_add_argument(prjm_eel_compiler_arg_list_t* arglist,
                                                             prjm_eel_compiler_node_t* arg)
{
    prjm_eel_compiler_arg_node_t* arg_node = calloc(1, sizeof(prjm_eel_compiler_arg_node_t));
    if (arg_node == NULL)
    {
        return NULL;
    }

    arg_node->node = arg;

    if (!arglist)
    {
        arglist = calloc(1, sizeof(prjm_eel_compiler_arg_list_t));
        if (!arglist)
        {
            free(arg_node);
            return NULL;
        }
        arglist->begin = arg_node;
    }
    else
    {
        arglist->end->next = arg_node;
    }

    arglist->end = arg_node;
    arglist->count++;

    return arglist;
}

prjm_eel_compiler_node_t* prjm_eel_compiler_create_function(prjm_eel_compiler_context_t* cctx,
                                                            const char* name,
                                                            prjm_eel_compiler_arg_list_t* arglist,
                                                            char** error)
{
    prjm_eel_function_def_t* func = prjm_eel_compiler_get_function(cctx, name);
    if (!func)
    {
        *error = strdup("Invalid function");
        return NULL;
    }

    if (func->arg_count != arglist->count)
    {
        *error = strdup("Invalid argument count");
        return NULL;
    }

    prjm_eel_compiler_node_t* node = prjm_eel_compiler_create_expression(cctx, func, arglist);

    return node;
}

prjm_eel_compiler_node_t* prjm_eel_compiler_create_variable(prjm_eel_compiler_context_t* cctx, const char* name)
{
    /* Find existing variable or create a new one */
    prjm_eel_variable_def_t* var = prjm_eel_register_variable(cctx, name, NULL);

    prjm_eel_function_def_t* var_func = prjm_eel_compiler_get_function(cctx, "/*var*/");
    prjm_eel_compiler_node_t* node = prjm_eel_compiler_create_expression_empty(var_func);

    prjm_eel_exptreenode_t* expr = node->tree_node;
    expr->var = var;

    return node;
}

prjm_eel_compiler_node_t* prjm_eel_compiler_create_expression_empty(prjm_eel_function_def_t* func)
{
    prjm_eel_exptreenode_t* expr = calloc(1, sizeof(prjm_eel_exptreenode_t));

    expr->func = func->func;
    expr->math_func = func->math_func;

    prjm_eel_compiler_node_t* node = calloc(1, sizeof(prjm_eel_compiler_node_t));

    node->type = PRJM_EEL_NODE_FUNC_EXPRESSION;
    node->is_const_expr = func->is_const_eval;
    node->is_state_changing = func->is_state_changing;
    node->tree_node = expr;

    return node;
}

prjm_eel_compiler_node_t* prjm_eel_compiler_create_expression(prjm_eel_compiler_context_t* cctx,
                                                              prjm_eel_function_def_t* func,
                                                              prjm_eel_compiler_arg_list_t* arglist)
{
    prjm_eel_exptreenode_t* expr = calloc(1, sizeof(prjm_eel_exptreenode_t));

    expr->func = func->func;
    expr->math_func = func->math_func;

    bool args_are_const_evaluable = true;
    bool args_are_state_changing = false;
    if (arglist && arglist->count > 0)
    {
        expr->args = calloc(arglist->count + 1, sizeof(prjm_eel_exptreenode_t*));

        prjm_eel_compiler_arg_node_t* arg = arglist->begin;
        prjm_eel_exptreenode_t** expr_arg = expr->args;
        while (arg)
        {
            /* Move expression to arguments */
            *expr_arg = arg->node->tree_node;
            arg->node->tree_node = NULL;

            /* If at least one arg is not const-evaluable, the function isn't. */
            args_are_const_evaluable = args_are_const_evaluable && arg->node->is_const_expr;
            /* If at least one arg is state-changing, the function is also. */
            args_are_state_changing = args_are_state_changing || arg->node->is_state_changing;

            expr_arg++;
            arg = arg->next;
        }

        prjm_eel_compiler_destroy_arglist(arglist);
    }

    prjm_eel_compiler_node_t* node = calloc(1, sizeof(prjm_eel_compiler_node_t));

    node->type = PRJM_EEL_NODE_FUNC_EXPRESSION;
    node->is_const_expr = args_are_const_evaluable && func->is_const_eval;
    node->is_state_changing = args_are_state_changing || func->is_state_changing;

    // Evaluate expression if constant-evaluable
    if (node->is_const_expr &&
        !node->is_state_changing)
    {
        printf("[Optimizer] Evaluating and replacing constant expression\n");
        prjm_eel_exptreenode_t* const_expr = calloc(1, sizeof(prjm_eel_exptreenode_t));
        prjm_eel_function_def_t* const_func = prjm_eel_compiler_get_function(cctx, "/*const*/");
        const_expr->func = const_func->func;

        float* value_ptr = &const_expr->value;
        expr->func(expr, &value_ptr);
        const_expr->value = *value_ptr;

        prjm_eel_destroy_exptreenode(expr);

        node->tree_node = const_expr;
        node->is_const_expr = const_func->is_const_eval;
        node->is_state_changing = const_func->is_state_changing;
    }
    else
    {
        node->tree_node = expr;
    }

    return node;
}

prjm_eel_compiler_node_t* prjm_eel_compiler_create_const_expression(prjm_eel_compiler_context_t* cctx,
                                                                    float value)
{
    prjm_eel_function_def_t* const_func = prjm_eel_compiler_get_function(cctx, "/*const*/");

    prjm_eel_exptreenode_t* const_expr = calloc(1, sizeof(prjm_eel_exptreenode_t));
    const_expr->func = const_func->func;
    const_expr->value = value;

    prjm_eel_compiler_node_t* node = calloc(1, sizeof(prjm_eel_compiler_node_t));
    node->type = PRJM_EEL_NODE_FUNC_EXPRESSION;
    node->tree_node = const_expr;
    node->is_const_expr = const_func->is_const_eval;
    node->is_state_changing = const_func->is_state_changing;

    return node;
}

prjm_eel_compiler_node_t* prjm_eel_compiler_add_instruction(prjm_eel_compiler_context_t* cctx,
                                                            prjm_eel_compiler_node_t* list,
                                                            prjm_eel_compiler_node_t* instruction)
{
    if (!list)
    {
        return NULL;
    }

    prjm_eel_compiler_node_t* node = list;

    /* Convert single expression to instruction list if needed. */
    if (node->type == PRJM_EEL_NODE_FUNC_EXPRESSION)
    {
        /* If previous instruction is not state-changing, we can remove it as it won't do
         * anything useful. Only the last expression's value may be of interest. */
        if (!node->is_state_changing)
        {
            printf("[Optimizer] Deleting previous non-state changing/uninteresting expression.\n");
            prjm_eel_compiler_destroy_node(list);
            return instruction;
        }

        prjm_eel_function_def_t* list_func = prjm_eel_compiler_get_function(cctx, "/*list*/");

        prjm_eel_compiler_node_t* new_node = prjm_eel_compiler_create_expression_empty(list_func);
        new_node->tree_node->list = malloc(sizeof(prjm_eel_exptreenode_list_item_t));
        new_node->tree_node->list->expr = list->tree_node;
        new_node->tree_node->list->next = NULL;
        new_node->type = PRJM_EEL_NODE_FUNC_INSTRUCTIONLIST;
        new_node->is_const_expr = list->is_const_expr;
        new_node->is_state_changing = list->is_state_changing;

        free(list);

        node = new_node;
    }

    prjm_eel_exptreenode_list_item_t* item = node->tree_node->list;
    while(item->next)
    {
        /* If last expression in the existing list is not state-changing, we can remove it as it won't do
         * anything useful. Only the last expression's value may be of interest. */
        if (!node->is_state_changing && !item->next->next)
        {
            printf("[Optimizer] Deleting previous non-state changing/uninteresting expression.\n");
            prjm_eel_destroy_exptreenode(item->next->expr);
            free(item->next);
            break;
        }

        item = item->next;
    }

    item->next = malloc(sizeof(prjm_eel_exptreenode_list_item_t));
    item->next->expr = instruction->tree_node;
    instruction->tree_node = NULL;

    /* Update const/state flags with last expression */
    node->is_const_expr = instruction->is_const_expr;
    node->is_state_changing = instruction->is_state_changing;

    free(instruction);

    return node;
}