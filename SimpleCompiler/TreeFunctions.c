#include "TreeFunctions.h"

#include <math.h>
#include <assert.h>

#define EULER_NUMBER_F 2.71828182846f

/**
 * @brief Function table.
 * Contains all available functions and information about their invocation. Most functions beginning
 * with _ are not really required because they're internals of ns-eel2, but as they can be used in
 * Milkdrop, we allow expressions to use them directly.
 */
static func_entry function_table[] = {
    { "_if", prjm_eel_func_if, 0, 3},
    { "_and", prjm_eel_func_band, 0, 2},
    { "_or", prjm_eel_func_bor, 0, 2},
    { "loop", prjm_eel_func_execute_loop, 0, 2},
    { "while", prjm_eel_func_execute_while, 0, 1},

    { "_not", prjm_eel_func_bnot, 0, 1},
    { "_equal", prjm_eel_func_equal, 0, 2},
    { "_noteq", prjm_eel_func_notequal, 0, 2},
    { "_below", prjm_eel_func_below, 0, 2},
    { "_above", prjm_eel_func_above, 0, 2},
    { "_beleq", prjm_eel_func_beloweq, 0, 2},
    { "_aboeq", prjm_eel_func_aboveeq, 0, 2},

    { "_set", prjm_eel_func_set, 0, 2},
    { "_mod", prjm_eel_func_mod, 0, 2},
    { "_mulop", prjm_eel_func_mulop, 0, 2},
    { "_divop", prjm_eel_func_divop, 0, 2},
    { "_orop", prjm_eel_func_orop, 0, 2},
    { "_andop", prjm_eel_func_andop, 0, 2},
    { "_addop", prjm_eel_func_addop, 0, 2},
    { "_subop", prjm_eel_func_subop, 0, 2},
    { "_modop", prjm_eel_func_modop, 0, 2},

    { "sin", prjm_eel_func_math_func1, sinf, 1 },
    { "cos", prjm_eel_func_math_func1, cosf, 1 },
    { "tan", prjm_eel_func_math_func1, tanf, 1 },
    { "asin", prjm_eel_func_math_func1, asinf, 1 },
    { "acos", prjm_eel_func_math_func1, acosf, 1 },
    { "atan", prjm_eel_func_math_func1, atanf, 1 },
    { "atan2", prjm_eel_func_math_func2, atan2f, 2 },
    { "sqr", prjm_eel_func_math_sqr, 0, 1 },
    { "sqrt", prjm_eel_func_math_func1, sqrtf, 1 },
    { "pow", prjm_eel_func_math_func2, powf, 2 },
    { "_powop", prjm_eel_func_powop, 0, 2 },
    { "exp", prjm_eel_func_math_func1, expf, 1 },

    { "log", prjm_eel_func_math_func1, logf, 1 },
    { "log10", prjm_eel_func_math_func1, log10f, 1 },
    { "abs", prjm_eel_func_abs, 0, 1 },
    { "min", prjm_eel_func_min, 0, 2 },
    { "max", prjm_eel_func_max, 0, 2 },
    { "sign", prjm_eel_func_sign, 0, 1 },
    { "rand", prjm_eel_func_rand, 0, 1 },
    { "floor", prjm_eel_func_math_func1, floorf, 1 },
    { "ceil", prjm_eel_func_math_func1, ceilf, 1 },
    { "invsqrt", prjm_eel_func_invsqrt, 0, 1 },
    { "sigmoid", prjm_eel_func_sigmoid, 0, 2 },

    { "band", prjm_eel_func_band, 0, 2 },
    { "bor", prjm_eel_func_bor, 0, 2 },

    { "exec2", prjm_eel_func_exec2, 0, 2 },
    { "exec3", prjm_eel_func_exec3, 0, 3 },
    { "_mem", prjm_eel_func_megabuf, 0, 1 },
    { "_gmem", prjm_eel_func_gmegabuf, 0, 1 },
    { "freembuf", prjm_eel_func_freembuf, 0, 1 },
    { "memcpy", prjm_eel_func_memcpy, 0, 3 },
    { "memset", prjm_eel_func_memset, 0, 3 }
};

/* Variable and constant access */
void prjm_eel_func_const(struct prjm_eel_exptreenode* ctx, float** ret_val)
{
    assert(ctx);
    assert(ctx->func);
    assert(ret_val);
    assert(*ret_val);
    (**ret_val) = ctx->value;
}

void prjm_eel_func_var(struct prjm_eel_exptreenode* ctx, float** ret_val)
{
    assert(ctx);
    assert(ctx->func);
    assert(ctx->var);
    assert(ret_val);
    assert(*ret_val);
    (*ret_val) = ctx->var->value_ptr;
}



/* Execution control structures */
void prjm_eel_func_execute_list(struct prjm_eel_exptreenode* ctx, float** ret_val)
{
    assert(ctx);
    assert(ctx->func);
    assert(ctx->next);
    assert(ret_val);
    assert(*ret_val);

    prjm_eel_exptreenode_t* cur_expr = ctx;
    while (cur_expr->next)
    {
        cur_expr = cur_expr->next;
        assert(cur_expr->func);
        cur_expr->func(cur_expr, ret_val);
    }
}

void prjm_eel_func_execute_loop(struct prjm_eel_exptreenode* ctx, float** ret_val)
{
    assert(ctx);
    assert(ctx->func);
    assert(ctx->arg1);
    assert(ctx->next);
    assert(ret_val);
    assert(*ret_val);
    float* loop_count_ptr = &ctx->value;
    ctx->arg1->func(ctx->arg1, &loop_count_ptr);

    int loop_count_int = (int) (*loop_count_ptr);
    /* Limit execution count */
    if (loop_count_int > 1000000)
    {
        loop_count_int = 1000000;
    }

    for (int i = 0; i < loop_count_int; i++)
    {
        prjm_eel_exptreenode_t* cur_expr = ctx;
        while (cur_expr->next)
        {
            cur_expr = cur_expr->next;
            assert(cur_expr->func);
            cur_expr->func(cur_expr, ret_val);
        }
    }
}

void prjm_eel_func_execute_while(struct prjm_eel_exptreenode* ctx, float** ret_val)
{
    assert(ctx);
    assert(ctx->func);
    assert(ctx->arg1);
    assert(ctx->next);
    assert(ret_val);
    assert(*ret_val);

    int loop_count_int = 0;
    do
    {
        prjm_eel_exptreenode_t* cur_expr = ctx;
        while (cur_expr->next)
        {
            cur_expr = cur_expr->next;
            assert(cur_expr->func);
            cur_expr->func(cur_expr, ret_val);
        }

        loop_count_int++;
    } while (*ret_val != 0 && loop_count_int < 1000000);
}

void prjm_eel_func_if(struct prjm_eel_exptreenode* ctx, float** ret_val)
{
    assert(ctx);
    assert(ctx->func);
    assert(ctx->arg1);
    assert(ctx->arg2);
    assert(ctx->arg3);
    assert(ret_val);
    assert(*ret_val);
    float* if_arg = &ctx->value;
    ctx->arg1->func(ctx->arg1, &if_arg);
    if ((*if_arg) != 0)
    {
        ctx->arg2->func(ctx->arg2, ret_val);
        return;
    }
    ctx->arg3->func(ctx->arg3, ret_val);
}

void prjm_eel_func_set(struct prjm_eel_exptreenode* ctx, float** ret_val)
{
    assert(ctx);
    assert(ctx->func);
    assert(ctx->arg1);
    assert(ret_val);
    assert(*ret_val);
    ctx->arg1->func(ctx->arg1, ret_val);
}


/* Operators */
void prjm_eel_func_add(struct prjm_eel_exptreenode* ctx, float** ret_val)
{
    assert(ctx);
    assert(ctx->func);
    assert(ctx->arg1);
    assert(ctx->arg2);
    assert(ret_val);
    assert(*ret_val);
    float val1 = .0f;
    float val2 = .0f;
    float* val1_ptr = &val1;
    float* val2_ptr = &val2;
    ctx->arg1->func(ctx->arg1, &val1_ptr);
    ctx->arg2->func(ctx->arg2, &val2_ptr);
    (**ret_val) = val1 + val2;
}

void prjm_eel_func_sub(struct prjm_eel_exptreenode* ctx, float** ret_val)
{
    assert(ctx);
    assert(ctx->func);
    assert(ctx->arg1);
    assert(ctx->arg2);
    assert(ret_val);
    assert(*ret_val);
    float val1 = .0f;
    float val2 = .0f;
    float* val1_ptr = &val1;
    float* val2_ptr = &val2;
    ctx->arg1->func(ctx->arg1, &val1_ptr);
    ctx->arg2->func(ctx->arg2, &val2_ptr);
    **ret_val = val1 - val2;
}

void prjm_eel_func_mul(struct prjm_eel_exptreenode* ctx, float** ret_val)
{
    assert(ctx);
    assert(ctx->func);
    assert(ctx->arg1);
    assert(ctx->arg2);
    assert(ret_val);
    assert(*ret_val);
    float val1 = .0f;
    float val2 = .0f;
    float* val1_ptr = &val1;
    float* val2_ptr = &val2;
    ctx->arg1->func(ctx->arg1, &val1_ptr);
    ctx->arg2->func(ctx->arg2, &val2_ptr);
    **ret_val = val1 * val2;
}

void prjm_eel_func_div(struct prjm_eel_exptreenode* ctx, float** ret_val)
{
    assert(ctx);
    assert(ctx->func);
    assert(ctx->arg1);
    assert(ctx->arg2);
    assert(ret_val);
    assert(*ret_val);
    float val1 = .0f;
    float val2 = .0f;
    float* val1_ptr = &val1;
    float* val2_ptr = &val2;
    ctx->arg1->func(ctx->arg1, &val1_ptr);
    ctx->arg2->func(ctx->arg2, &val2_ptr);
    **ret_val = val1 / val2;
}

void prjm_eel_func_mod(struct prjm_eel_exptreenode* ctx, float** ret_val)
{
    assert(ctx);
    assert(ctx->func);
    assert(ctx->arg1);
    assert(ctx->arg2);
    assert(ret_val);
    assert(*ret_val);
    float val1 = .0f;
    float val2 = .0f;
    float* val1_ptr = &val1;
    float* val2_ptr = &val2;
    ctx->arg1->func(ctx->arg1, &val1_ptr);
    ctx->arg2->func(ctx->arg2, &val2_ptr);
    **ret_val = (float) ((int) val1 % (int) val2);
}

void prjm_eel_func_band(struct prjm_eel_exptreenode* ctx, float** ret_val)
{
    assert(ctx);
    assert(ctx->func);
    assert(ctx->arg1);
    assert(ctx->arg2);
    assert(ret_val);
    assert(*ret_val);
    float val1 = .0f;
    float val2 = .0f;
    float* val1_ptr = &val1;
    float* val2_ptr = &val2;
    ctx->arg1->func(ctx->arg1, &val1_ptr);
    ctx->arg2->func(ctx->arg2, &val2_ptr);
    **ret_val = (float) ((int) val1 & (int) val2);
}

void prjm_eel_func_bor(struct prjm_eel_exptreenode* ctx, float** ret_val)
{
    assert(ctx);
    assert(ctx->func);
    assert(ctx->arg1);
    assert(ctx->arg2);
    assert(ret_val);
    assert(*ret_val);
    float val1 = .0f;
    float val2 = .0f;
    float* val1_ptr = &val1;
    float* val2_ptr = &val2;
    ctx->arg1->func(ctx->arg1, &val1_ptr);
    ctx->arg2->func(ctx->arg2, &val2_ptr);
    **ret_val = (float) ((int) val1 | (int) val2);
}


void prjm_eel_func_addop(struct prjm_eel_exptreenode* ctx, float** ret_val)
{
    assert(ctx);
    assert(ctx->func);
    assert(ctx->arg1);
    assert(ctx->arg2);
    assert(ret_val);
    assert(*ret_val);
    float val2 = .0f;
    float* val1_ptr = &ctx->value;
    float* val2_ptr = &val2;
    ctx->arg1->func(ctx->arg1, &val1_ptr);
    ctx->arg2->func(ctx->arg2, &val2_ptr);
    (*val1_ptr) = *val1_ptr + *val2_ptr;
    (*ret_val) = val1_ptr;
}

void prjm_eel_func_subop(struct prjm_eel_exptreenode* ctx, float** ret_val)
{
    assert(ctx);
    assert(ctx->func);
    assert(ctx->arg1);
    assert(ctx->arg2);
    assert(ret_val);
    assert(*ret_val);
    float val2 = .0f;
    float* val1_ptr = &ctx->value;
    float* val2_ptr = &val2;
    ctx->arg1->func(ctx->arg1, &val1_ptr);
    ctx->arg2->func(ctx->arg2, &val2_ptr);
    (*val1_ptr) = *val1_ptr - *val2_ptr;
    (*ret_val) = val1_ptr;
}

void prjm_eel_func_mulop(struct prjm_eel_exptreenode* ctx, float** ret_val)
{
    assert(ctx);
    assert(ctx->func);
    assert(ctx->arg1);
    assert(ctx->arg2);
    assert(ret_val);
    assert(*ret_val);
    float val2 = .0f;
    float* val1_ptr = &ctx->value;
    float* val2_ptr = &val2;
    ctx->arg1->func(ctx->arg1, &val1_ptr);
    ctx->arg2->func(ctx->arg2, &val2_ptr);
    (*val1_ptr) = *val1_ptr * *val2_ptr;
    (*ret_val) = val1_ptr;
}

void prjm_eel_func_divop(struct prjm_eel_exptreenode* ctx, float** ret_val)
{
    assert(ctx);
    assert(ctx->func);
    assert(ctx->arg1);
    assert(ctx->arg2);
    assert(ret_val);
    assert(*ret_val);
    float val2 = .0f;
    float* val1_ptr = &ctx->value;
    float* val2_ptr = &val2;
    ctx->arg1->func(ctx->arg1, &val1_ptr);
    ctx->arg2->func(ctx->arg2, &val2_ptr);
    (*val1_ptr) = *val1_ptr / *val2_ptr;
    (*ret_val) = val1_ptr;
}
void prjm_eel_func_modop(struct prjm_eel_exptreenode* ctx, float** ret_val)
{
    assert(ctx);
    assert(ctx->func);
    assert(ctx->arg1);
    assert(ctx->arg2);
    assert(ret_val);
    assert(*ret_val);
    float val2 = .0f;
    float* val1_ptr = &ctx->value;
    float* val2_ptr = &val2;
    ctx->arg1->func(ctx->arg1, &val1_ptr);
    ctx->arg2->func(ctx->arg2, &val2_ptr);
    (*val1_ptr) = (float) ((int) (*val1_ptr) % (int) (*val2_ptr));
    (*ret_val) = val1_ptr;
}



/* Math functions */
void prjm_eel_func_math_func1(struct prjm_eel_exptreenode* ctx, float** ret_val)
{
    assert(ctx);
    assert(ctx->func);
    assert(ctx->math_func);
    assert(ctx->arg1);
    assert(ret_val);
    assert(*ret_val);
    float* math_arg_ptr = &ctx->value;
    ctx->arg1->func(ctx->arg1, &math_arg_ptr);
    (**ret_val) = ((prjm_eel_math_func1*)ctx->math_func)(*math_arg_ptr);
}

void prjm_eel_func_math_func2(struct prjm_eel_exptreenode* ctx, float** ret_val)
{
    assert(ctx);
    assert(ctx->func);
    assert(ctx->math_func);
    assert(ctx->arg1);
    assert(ctx->arg2);
    assert(ret_val);
    assert(*ret_val);
    float math_arg2 = .0f;
    float* math_arg1_ptr = &ctx->value;
    float* math_arg2_ptr = &math_arg2;
    ctx->arg1->func(ctx->arg1, &math_arg1_ptr);
    ctx->arg1->func(ctx->arg2, &math_arg2_ptr);
    (**ret_val) = ((prjm_eel_math_func2*)ctx->math_func)(*math_arg1_ptr, *math_arg2_ptr);
}

void prjm_eel_func_sigmoid(struct prjm_eel_exptreenode* ctx, float** ret_val)
{
    assert(ctx);
    assert(ctx->func);
    assert(ctx->math_func);
    assert(ctx->arg1);
    assert(ctx->arg2);
    assert(ret_val);
    assert(*ret_val);
    float val2 = .0f;
    float* math_arg1_ptr = &ctx->value;
    float* math_arg2_ptr = &val2;
    ctx->arg1->func(ctx->arg1, &math_arg1_ptr);
    ctx->arg1->func(ctx->arg2, &math_arg2_ptr);
    (**ret_val) =  (1 / (1 + powf(*math_arg1_ptr, -(*math_arg2_ptr))));
}
