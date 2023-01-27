#pragma once

#include "ExpressionTree.h"

typedef struct
{
    const char* name;
    prjm_eel_expr_func_t* func;
    void* math_func;
    int params;
} func_entry;


/* Variable and constant access */
void prjm_eel_func_const(struct prjm_eel_exptreenode* ctx, float** ret_val);
void prjm_eel_func_var(struct prjm_eel_exptreenode* ctx, float** ret_val);

/* Execution control structures */
void prjm_eel_func_execute_list(struct prjm_eel_exptreenode* ctx, float** ret_val);
void prjm_eel_func_execute_loop(struct prjm_eel_exptreenode* ctx, float** ret_val);
void prjm_eel_func_execute_while(struct prjm_eel_exptreenode* ctx, float** ret_val);
void prjm_eel_func_if(struct prjm_eel_exptreenode* ctx, float** ret_val);

/* Assignment function */
void prjm_eel_func_set(struct prjm_eel_exptreenode* ctx, float** ret_val);

/* Operators */
void prjm_eel_func_add(struct prjm_eel_exptreenode* ctx, float** ret_val);
void prjm_eel_func_sub(struct prjm_eel_exptreenode* ctx, float** ret_val);
void prjm_eel_func_mul(struct prjm_eel_exptreenode* ctx, float** ret_val);
void prjm_eel_func_div(struct prjm_eel_exptreenode* ctx, float** ret_val);
void prjm_eel_func_mod(struct prjm_eel_exptreenode* ctx, float** ret_val);
void prjm_eel_func_band(struct prjm_eel_exptreenode* ctx, float** ret_val);
void prjm_eel_func_bor(struct prjm_eel_exptreenode* ctx, float** ret_val);

/* Compound assignment operators */
void prjm_eel_func_addop(struct prjm_eel_exptreenode* ctx, float** ret_val);
void prjm_eel_func_subop(struct prjm_eel_exptreenode* ctx, float** ret_val);
void prjm_eel_func_mulop(struct prjm_eel_exptreenode* ctx, float** ret_val);
void prjm_eel_func_divop(struct prjm_eel_exptreenode* ctx, float** ret_val);
void prjm_eel_func_modop(struct prjm_eel_exptreenode* ctx, float** ret_val);

void prjm_eel_func_math_func1(struct prjm_eel_exptreenode* ctx, float** ret_val);
void prjm_eel_func_math_func2(struct prjm_eel_exptreenode* ctx, float** ret_val);
void prjm_eel_func_sigmoid(struct prjm_eel_exptreenode* ctx, float** ret_val);