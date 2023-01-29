/**
 * @file TreeFunctions.h
 * @brief Declares all intrinsic projectM-EvalLib functions and types.
 */
#pragma once

#include "ExpressionTree.h"

/**
 * @brief Structure containing information about an available function implementation.
 * This struct is used to fill the intrinsic function table and is used to add additional,
 * externally-defined functions to the parser.
 */
typedef struct
{
    const char* name; /*!< The lower-case name of the function in the expression syntax */
    prjm_eel_expr_func_t* func; /*!< A pointer to the function implementation */
    void* math_func; /*!< When using math_func1 or math_func2 in @a func, a pointer to the C library math function. */
    int params; /*!< Number of accepted parameters, 1 to 3. */
} func_entry;

/**
 * @brief Abbreviates parser function declaration.
 * @param func The name of the function, appended to "prjm_eel_func_".
 */
#define prjm_eel_function_decl(func) \
    void prjm_eel_func_ ## func(struct prjm_eel_exptreenode* ctx, float** ret_val)

/* Variable and constant access */
prjm_eel_function_decl(const);
prjm_eel_function_decl(var);

/* Execution control structures */
prjm_eel_function_decl(list);
prjm_eel_function_decl(execute_loop);
prjm_eel_function_decl(execute_while);
prjm_eel_function_decl(if);
prjm_eel_function_decl(exec2);
prjm_eel_function_decl(exec3);

/* Assignment function */
prjm_eel_function_decl(set);

/* Memory access function */
prjm_eel_function_decl(megabuf);
prjm_eel_function_decl(gmegabuf);
prjm_eel_function_decl(freembuf);
prjm_eel_function_decl(memcpy);
prjm_eel_function_decl(memset);

/* Operators */
prjm_eel_function_decl(bnot);
prjm_eel_function_decl(equal);
prjm_eel_function_decl(notequal);
prjm_eel_function_decl(below);
prjm_eel_function_decl(above);
prjm_eel_function_decl(beloweq);
prjm_eel_function_decl(aboveeq);

prjm_eel_function_decl(add);
prjm_eel_function_decl(sub);
prjm_eel_function_decl(mul);
prjm_eel_function_decl(div);
prjm_eel_function_decl(mod);
prjm_eel_function_decl(band);
prjm_eel_function_decl(bor);

/* Compound assignment operators */
prjm_eel_function_decl(addop);
prjm_eel_function_decl(subop);
prjm_eel_function_decl(mulop);
prjm_eel_function_decl(divop);
prjm_eel_function_decl(modop);
prjm_eel_function_decl(orop);
prjm_eel_function_decl(andop);
prjm_eel_function_decl(powop);

/* Mathematical functions */
prjm_eel_function_decl(math_func1);
prjm_eel_function_decl(math_func2);
prjm_eel_function_decl(sigmoid);
prjm_eel_function_decl(sqr);
prjm_eel_function_decl(abs);
prjm_eel_function_decl(min);
prjm_eel_function_decl(max);
prjm_eel_function_decl(sign);
prjm_eel_function_decl(rand);
prjm_eel_function_decl(invsqrt);
