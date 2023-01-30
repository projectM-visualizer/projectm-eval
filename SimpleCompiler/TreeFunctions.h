/**
 * @file TreeFunctions.h
 * @brief Declares all intrinsic projectM-EvalLib functions and types.
 */
#pragma once

#include "CompilerTypes.h"
#include "ExpressionTree.h"

/**
 * @brief Returns the list with the built-in function table.
 * The list must not be freed, as it points to the internal static data.
 * @param list A pointer which will receive the list reference.
 * @param count The number of elements inserted into the list.
 */
void prjm_eel_intrinsic_functions(prjm_eel_intrinsic_function_list_ptr list, int* count);

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
prjm_eel_function_decl(execute_list);
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
prjm_eel_function_decl(neg);

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
