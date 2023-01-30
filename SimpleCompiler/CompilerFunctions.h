#pragma once

#include "CompilerTypes.h"
#include "Compiler.h"

#define PRJM_EEL_FUNC(ret, name, args) {\
    char* errval = 0; \
    ret = prjm_eel_compiler_create_function(cctx, name, args, &errval); \
    if(errval) { yyerror(&yyloc, cctx, scanner, errval); free(errval); }}

#define PRJM_EEL_FUNC1(ret, name, arg1) {\
    char* errval = 0;                   \
    prjm_eel_compiler_arg_list_t* arglist = prjm_eel_compiler_add_argument(NULL, arg1); \
    ret = prjm_eel_compiler_create_function(cctx, name, arglist, &errval); \
    if(errval) { yyerror(&yyloc, cctx, scanner, errval); free(errval); }}

#define PRJM_EEL_FUNC2(ret, name, arg1, arg2) {\
    char* errval = 0; \
    prjm_eel_compiler_arg_list_t* arglist = prjm_eel_compiler_add_argument(NULL, arg1); \
    arglist = prjm_eel_compiler_add_argument(arglist, arg2); \
    ret = prjm_eel_compiler_create_function(cctx, name, arglist, &errval); \
    if(errval) { yyerror(&yyloc, cctx, scanner, errval); free(errval); }}

#define PRJM_EEL_FUNC3(ret, name, arg1, arg2, arg3) {\
    char* errval = 0; \
    prjm_eel_compiler_arg_list_t* arglist = prjm_eel_compiler_add_argument(NULL, arg1); \
    arglist = prjm_eel_compiler_add_argument(arglist, arg2);                            \
    arglist = prjm_eel_compiler_add_argument(arglist, arg3); \
    ret = prjm_eel_compiler_create_function(cctx, name, arglist, &errval); \
    if(errval) { yyerror(&yyloc, cctx, scanner, errval); free(errval); }}

/**
 * @brief Frees the given argument list container and items.
 * Also destroys any nodes and their stored expressions. If the expressions are still used elsewhere,
 * set the pointers to NULL before calling this method.
 * @param arglist The argument list to destroy.
 */
void prjm_eel_compiler_destroy_arglist(prjm_eel_compiler_arg_list_t* arglist);

/**
 * @brief Frees the given node and any contained expressions.
 * If some of the expressions are still used elsewhere, set these pointers to NULL before calling this method.
 * @param node The node to destroy.
 */
void prjm_eel_compiler_destroy_node(prjm_eel_compiler_node_t* node);

/**
 * @brief Determines if the given name is a function name.
 * @param cctx The compile context.
 * @param name The function name to look up.
 * @return true if the name represents a function, false if not (parse as variable).
 */
bool prjm_eel_compiler_name_is_function(prjm_eel_compiler_context_t* cctx, const char* name);

prjm_eel_function_def_t* prjm_eel_compiler_get_function(prjm_eel_compiler_context_t* cctx, const char* name);

prjm_eel_compiler_arg_list_t* prjm_eel_compiler_add_argument(prjm_eel_compiler_arg_list_t* arglist,
                                                             prjm_eel_compiler_node_t* arg);

prjm_eel_compiler_node_t* prjm_eel_compiler_create_function(prjm_eel_compiler_context_t* cctx,
                                                            const char* name,
                                                            prjm_eel_compiler_arg_list_t* arglist,
                                                            char** error);

/**
 * @brief Creates a new node pointing to a new or existing varable.
 * @param name The name of the variable to create or returb.
 * @return The new VAR node.
 */
prjm_eel_compiler_node_t* prjm_eel_compiler_create_variable(prjm_eel_compiler_context_t* cctx, const char* name);

/**
 * @brief Creates a new node with an empty expression, e.g. no args.
 * @param func The function to insert into the expression.
 * @return The new node.
 */
prjm_eel_compiler_node_t* prjm_eel_compiler_create_expression_empty(prjm_eel_function_def_t* func);

/**
 * @brief Creates a new compiler node with an expression using the function and arguments given.
 * Will also evaluate the function and replace the expression with a const value if possible.
 * @note The passed @a arglist is destroyed by this function. No further cleanup required, don't use the pointer.
 * @param cctx The compiler context.
 * @param func The function to execute.
 * @param arglist A list of arguments for the function. Argument count must match the function.
 * @return A new node with the freshly generated expression.
 */
prjm_eel_compiler_node_t* prjm_eel_compiler_create_expression(prjm_eel_compiler_context_t* cctx,
                                                              prjm_eel_function_def_t* func,
                                                              prjm_eel_compiler_arg_list_t* arglist);

prjm_eel_compiler_node_t* prjm_eel_compiler_create_const_expression(prjm_eel_compiler_context_t* cctx,
                                                                    float value);

prjm_eel_compiler_node_t* prjm_eel_compiler_add_instruction(prjm_eel_compiler_context_t* cctx,
                                                            prjm_eel_compiler_node_t* list,
                                                            prjm_eel_compiler_node_t* instruction);