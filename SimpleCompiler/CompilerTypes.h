#pragma once

#include <TreeFunctions.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>  /* For printf, etc. */
#include <stdio.h>  /* For printf, etc. */
#include <math.h>   /* For pow, used in the grammar. */

typedef enum prjm_eel_compiler_node_type
{
    PRJM_EEL_NODE_FUNC_EXPRESSION,
    PRJM_EEL_NODE_FUNC_ARGLIST
} prjm_eel_compiler_node_type_t;


typedef struct prjm_eel_compiler_node
{
    prjm_eel_compiler_node_type_t type; /*!< Node type. Mostly expression, but can contain other types as well. */
    prjm_eel_exptreenode_t* tree_node; /*!< Generated tree node for this expression */
    bool is_const_expr; /*!< If true, this node and all sub nodes only consist of constant expressions, e.g. no variables used */
} prjm_eel_compiler_node_t;

typedef struct prjm_eel_compiler_arg_node
{
    prjm_eel_compiler_node_t* node; /*!< Expression for this argument. */
    struct prjm_eel_compiler_arg_node* next; /*!< Next argument in list. */
} prjm_eel_compiler_arg_node_t;

typedef struct prjm_eel_compiler_arg_list
{
    prjm_eel_compiler_arg_node_t* begin; /*!< First argument in the list. */
    prjm_eel_compiler_arg_node_t* end; /*!< Last argument in the list. */
} prjm_eel_compiler_arg_list_t;

typedef struct
{
    prjm_eel_function_list_t functions;
    prjm_eel_compiler_arg_node_t* begin; /*!< First argument in the list. */
    prjm_eel_compiler_arg_node_t* end; /*!< Last argument in the list. */
} prjm_eel_compiler_context_t;

#ifdef __cplusplus
}
#endif
