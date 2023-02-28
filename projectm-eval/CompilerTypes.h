#pragma once

#include "api/projectm-eval.h"

#include <stdbool.h>


struct prjm_eel_exptreenode;

/**
 * @brief Math function pointer with one argument.
 */
typedef PRJM_EVAL_F (prjm_eel_math_func1)(PRJM_EVAL_F);

/**
 * @brief Math function pointer with two arguments.
 */
typedef PRJM_EVAL_F (prjm_eel_math_func2)(PRJM_EVAL_F, PRJM_EVAL_F);

/**
 * @brief Math function pointer with three arguments.
 */
typedef PRJM_EVAL_F (prjm_eel_math_func3)(PRJM_EVAL_F, PRJM_EVAL_F, PRJM_EVAL_F);

/**
 * @brief Node function for a single expression.
 */
typedef void (prjm_eel_expr_func_t)(struct prjm_eel_exptreenode* ctx, PRJM_EVAL_F** ret_val);

/**
 * @brief Structure containing information about an available function implementation.
 * This struct is used to fill the intrinsic function table and is used to add additional,
 * externally-defined functions to the parser.
 */
typedef struct prjm_eel_function_def
{
    char* name; /*!< The lower-case name of the function in the expression syntax */
    prjm_eel_expr_func_t* func; /*!< A pointer to the function implementation */
    int arg_count; /*!< Number of accepted parameters, 1 to 3. */
    bool is_const_eval; /*!< If true, the function can be evaluated to a constant value at compile time. */
    bool is_state_changing; /*!< If true, the function will change the execution state (set memory) */
} prjm_eel_function_def_t;

typedef struct prjm_eel_function_list_item
{
    prjm_eel_function_def_t* function;
    struct prjm_eel_function_list_item* next;
} prjm_eel_function_list_item_t;

typedef struct
{
    prjm_eel_function_list_item_t* first;
} prjm_eel_function_list_t;

typedef const prjm_eel_function_def_t* prjm_eel_intrinsic_function_list;
typedef prjm_eel_intrinsic_function_list* prjm_eel_intrinsic_function_list_ptr;


typedef struct prjm_eel_variable_def
{
    char* name; /*!< The lower-case name of the variable in the expression syntax. */
    PRJM_EVAL_F value; /*!< The internal value of the variable. */
} prjm_eel_variable_def_t;

typedef struct prjm_eel_variable_entry
{
    prjm_eel_variable_def_t* variable;
    struct prjm_eel_variable_entry* next;
} prjm_eel_variable_entry_t;

typedef struct
{
    prjm_eel_variable_entry_t* first;
} prjm_eel_variable_list_t;

struct prjm_eel_exptreenode;

typedef struct prjm_eel_exptreenode_list_item
{
    struct prjm_eel_exptreenode* expr;
    struct prjm_eel_exptreenode_list_item* next;
} prjm_eel_exptreenode_list_item_t;

/**
 * @brief A single function, variable or constant in the expression tree.
 * The assigned function will determine how to access the other members.
 */
typedef struct prjm_eel_exptreenode
{
    prjm_eel_expr_func_t* func;
    PRJM_EVAL_F value; /*!< A constant, numerical value. Also used as temp value. */
    union
    {
        PRJM_EVAL_F* var; /*!< Variable reference. */
        projectm_eval_mem_buffer memory_buffer; /*!< megabuf/gmegabuf memory block. */
    };
    struct prjm_eel_exptreenode** args; /*!< Function arguments. Last element must be a NULL pointer*/
    prjm_eel_exptreenode_list_item_t* list;  /*!< Next argument in the instruction list. */
} prjm_eel_exptreenode_t;


typedef enum prjm_eel_compiler_node_type
{
    PRJM_EEL_NODE_FUNC_EXPRESSION,
    PRJM_EEL_NODE_FUNC_INSTRUCTIONLIST
} prjm_eel_compiler_node_type_t;


typedef struct prjm_eel_compiler_node
{
    prjm_eel_compiler_node_type_t type; /*!< Node type. Mostly expression, but can contain other types as well. */
    prjm_eel_exptreenode_t* tree_node; /*!< Generated tree node for this expression */
    bool instr_is_const_expr; /*!< If true, this node and all sub nodes only consist of constant expressions, e.g. no variables used */
    bool instr_is_state_changing; /*!< If true, the function will change the execution state (set memory) */
    bool list_is_const_expr; /*!< If true, the instruction list only consists of constant expressions, e.g. no variables used */
    bool list_is_state_changing; /*!< If true, at least one node in the instruction list will change the execution state (set memory) */
} prjm_eel_compiler_node_t;

typedef struct prjm_eel_compiler_arg_item
{
    prjm_eel_compiler_node_t* node; /*!< Expression for this argument. */
    struct prjm_eel_compiler_arg_item* next; /*!< Next argument in list. */
} prjm_eel_compiler_arg_node_t;

typedef struct prjm_eel_compiler_arg_list
{
    int count; /*!< Argument count in this list */
    prjm_eel_compiler_arg_node_t* begin; /*!< First argument in the list. */
    prjm_eel_compiler_arg_node_t* end; /*!< Last argument in the list. */
} prjm_eel_compiler_arg_list_t;

typedef struct
{
    char* error;
    int line;
    int column_start;
    int column_end;
} prjm_eel_compiler_error_t;

typedef struct projectm_eval_context
{
    prjm_eel_function_list_t functions; /*!< Functions available to this context. Initialized with the intrinsics table. */
    prjm_eel_variable_list_t variables; /*!< List of registered variables in this context. */
    PRJM_EVAL_F (*global_variables)[100]; /*!< Pointer to array with 100 global variables, reg00 to reg99. */
    projectm_eval_mem_buffer memory; /*!< The context-local memory buffer, referred to as megabuf. */
    projectm_eval_mem_buffer global_memory; /*!< The global memory buffer, referred to as gmegabuf. */
    prjm_eel_compiler_error_t error; /*!< Holds information about the last compile error. */
    prjm_eel_exptreenode_t* compile_result; /*!< The result of the last compilation. Used temporarily during compilation. */
} prjm_eel_compiler_context_t;

typedef struct
{
    prjm_eel_exptreenode_t* program;
    prjm_eel_compiler_context_t* cctx;
} prjm_eel_program_t;
