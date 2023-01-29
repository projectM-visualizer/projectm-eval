/**
 * @file TreeFunctions.c
 * @brief Implements all intrinsic projectM-EvalLib functions and helpers.
 */
#include "TreeFunctions.h"

#include <math.h>
#include <assert.h>
#include <stdint.h>

/**
 * @brief projectM-EvalLib intrinsic Function table.
 * Contains all predefined functions and information about their invocation. Most functions beginning
 * with _ are not really required because they're internals of ns-eel2, but as they can be used in
 * Milkdrop, we also allow expressions to call them directly.
 */
static prjm_eel_function_def_t intrinsic_function_table[] = {
    { "if",       prjm_eel_func_if,            0,      3, true },
    { "_if",      prjm_eel_func_if,            0,      3, true },
    { "_and",     prjm_eel_func_band,          0,      2, true },
    { "_or",      prjm_eel_func_bor,           0,      2, true },
    { "loop",     prjm_eel_func_execute_loop,  0,      2, true },
    { "while",    prjm_eel_func_execute_while, 0,      1, true },

    { "_not",     prjm_eel_func_bnot,          0,      1, true },
    { "_equal",   prjm_eel_func_equal,         0,      2, true },
    { "_noteq",   prjm_eel_func_notequal,      0,      2, true },
    { "_below",   prjm_eel_func_below,         0,      2, true },
    { "_above",   prjm_eel_func_above,         0,      2, true },
    { "_beleq",   prjm_eel_func_beloweq,       0,      2, true },
    { "_aboeq",   prjm_eel_func_aboveeq,       0,      2, true },

    { "_set",     prjm_eel_func_set,           0,      2, false },
    { "_mod",     prjm_eel_func_mod,           0,      2, true },
    { "_mulop",   prjm_eel_func_mulop,         0,      2, true },
    { "_divop",   prjm_eel_func_divop,         0,      2, true },
    { "_orop",    prjm_eel_func_orop,          0,      2, true },
    { "_andop",   prjm_eel_func_andop,         0,      2, true },
    { "_addop",   prjm_eel_func_addop,         0,      2, true },
    { "_subop",   prjm_eel_func_subop,         0,      2, true },
    { "_modop",   prjm_eel_func_modop,         0,      2, true },

    { "sin",      prjm_eel_func_math_func1,    sinf,   1, true },
    { "cos",      prjm_eel_func_math_func1,    cosf,   1, true },
    { "tan",      prjm_eel_func_math_func1,    tanf,   1, true },
    { "asin",     prjm_eel_func_math_func1,    asinf,  1, true },
    { "acos",     prjm_eel_func_math_func1,    acosf,  1, true },
    { "atan",     prjm_eel_func_math_func1,    atanf,  1, true },
    { "atan2",    prjm_eel_func_math_func2,    atan2f, 2, true },
    { "sqr",      prjm_eel_func_sqr,           0,      1, true },
    { "sqrt",     prjm_eel_func_math_func1,    sqrtf,  1, true },
    { "pow",      prjm_eel_func_math_func2,    powf,   2, true },
    { "_powop",   prjm_eel_func_powop,         0,      2, true },
    { "exp",      prjm_eel_func_math_func1,    expf,   1, true },

    { "log",      prjm_eel_func_math_func1,    logf,   1, true },
    { "log10",    prjm_eel_func_math_func1,    log10f, 1, true },
    { "abs",      prjm_eel_func_abs,           0,      1, true },
    { "min",      prjm_eel_func_min,           0,      2, true },
    { "max",      prjm_eel_func_max,           0,      2, true },
    { "sign",     prjm_eel_func_sign,          0,      1, true },
    { "rand",     prjm_eel_func_rand,          0,      1, false },
    { "floor",    prjm_eel_func_math_func1,    floorf, 1, true },
    { "ceil",     prjm_eel_func_math_func1,    ceilf,  1, true },
    { "invsqrt",  prjm_eel_func_invsqrt,       0,      1, true },
    { "sigmoid",  prjm_eel_func_sigmoid,       0,      2, true },

    { "band",     prjm_eel_func_band,          0,      2, true },
    { "bor",      prjm_eel_func_bor,           0,      2, true },

    { "exec2",    prjm_eel_func_exec2,         0,      2, true },
    { "exec3",    prjm_eel_func_exec3,         0,      3, true },
    { "_mem",     prjm_eel_func_megabuf,       0,      1, false },
    { "_gmem",    prjm_eel_func_gmegabuf,      0,      1, false },
    { "freembuf", prjm_eel_func_freembuf,      0,      1, false },
    { "memcpy",   prjm_eel_func_memcpy,        0,      3, false },
    { "memset",   prjm_eel_func_memset,        0,      3, false }
};


/* Helper functions and defines */
#define assert_valid_ctx() \
        assert(ctx); \
        assert(ret_val); \
        assert(*ret_val); \
        assert(ctx->func)

/**
 * Invokes the expression function of the indexed argument
 */
#define invoke_arg(argnum, retval) \
    assert(ctx->arg ## argnum);    \
    ctx->arg ## argnum->func(ctx->arg ## argnum, retval)

/**
 * Assigns the value as a constant float return value.
 * @param value The expression or value to assign. Must evaluate to float.
 */
#define assign_ret_val(value) \
    (**ret_val) = value

/**
 * Assigns the value as a constant float return value.
 * @param ref The reference pointer to assign. Must evaluate to float*.
 */
#define assign_ret_ref(ref) \
    (*ret_val) = ref

/* Used in genrand_int32 */
#define N 624
#define M 397
#define MATRIX_A 0x9908b0dfUL   /* constant vector a */
#define UPPER_MASK 0x80000000UL /* most significant w-r bits */
#define LOWER_MASK 0x7fffffffUL /* least significant r bits */

/* This is Milkdrop's original rand() implementation. */
static unsigned int prjm_eel_genrand_int32(void)
{
    unsigned int y;
    static unsigned int mag01[2] = { 0x0UL, MATRIX_A };
    /* mag01[x] = x * MATRIX_A  for x=0,1 */

    static unsigned int mt[N]; /* the array for the state vector  */
    static int mti; /* mti==N+1 means mt[N] is not initialized */


    if (!mti)
    {
        unsigned int s = 0x4141f00d;
        mt[0] = s & 0xffffffffUL;
        for (mti = 1; mti < N; mti++)
        {
            mt[mti] =
                (1812433253UL * (mt[mti - 1] ^ (mt[mti - 1] >> 30)) + mti);

            /* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
            /* In the previous versions, MSBs of the seed affect   */
            /* only MSBs of the array mt[].                        */
            /* 2002/01/09 modified by Makoto Matsumoto             */
            mt[mti] &= 0xffffffffUL;
            /* for >32 bit machines */
        }
    }

    if (mti >= N)
    { /* generate N words at one time */
        int kk;

        for (kk = 0; kk < N - M; kk++)
        {
            y = (mt[kk] & UPPER_MASK) | (mt[kk + 1] & LOWER_MASK);
            mt[kk] = mt[kk + M] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        for (; kk < N - 1; kk++)
        {
            y = (mt[kk] & UPPER_MASK) | (mt[kk + 1] & LOWER_MASK);
            mt[kk] = mt[kk + (M - N)] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        y = (mt[N - 1] & UPPER_MASK) | (mt[0] & LOWER_MASK);
        mt[N - 1] = mt[M - 1] ^ (y >> 1) ^ mag01[y & 0x1UL];

        mti = 0;
    }

    y = mt[mti++];

    /* Tempering */
    y ^= (y >> 11);
    y ^= (y << 7) & 0x9d2c5680UL;
    y ^= (y << 15) & 0xefc60000UL;
    y ^= (y >> 18);

    return y;
}

/* Variable and constant access */
prjm_eel_function_decl(const)
{
    assert_valid_ctx();

    assign_ret_val(ctx->value);
}

prjm_eel_function_decl(var)
{
    assert_valid_ctx();
    assert(ctx->var);
    assert(ctx->var->value_ptr);

    assign_ret_ref(ctx->var->value_ptr);
}


/* Execution control structures */
prjm_eel_function_decl(execute_list)
{
    assert_valid_ctx();
    assert(ctx->next);

    ctx->value = .0f;
    float* value_ptr = &ctx->value;
    prjm_eel_exptreenode_t* cur_expr = ctx;
    while (cur_expr->next)
    {
        ctx->value = .0f;
        value_ptr = &ctx->value;
        cur_expr = cur_expr->next;
        assert(cur_expr->func);
        cur_expr->func(cur_expr, &value_ptr);
    }

    assign_ret_ref(value_ptr);
}

prjm_eel_function_decl(execute_loop)
{
    assert_valid_ctx();
    assert(ctx->next);

    ctx->value = .0f;
    float* value_ptr = &ctx->value;
    invoke_arg(1, &value_ptr);

    int loop_count_int = (int) (*value_ptr);
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
            ctx->value = .0f;
            value_ptr = &ctx->value;
            cur_expr = cur_expr->next;
            assert(cur_expr->func);
            cur_expr->func(cur_expr, &value_ptr);
        }
    }

    assign_ret_ref(value_ptr);
}

prjm_eel_function_decl(execute_while)
{
    assert_valid_ctx();
    assert(ctx->next);

    ctx->value = .0f;
    float* value_ptr = &ctx->value;
    int loop_count_int = 0;
    do
    {
        prjm_eel_exptreenode_t* cur_expr = ctx;
        while (cur_expr->next)
        {
            ctx->value = .0f;
            value_ptr = &ctx->value;
            cur_expr = cur_expr->next;
            assert(cur_expr->func);
            cur_expr->func(cur_expr, &value_ptr);
        }

        loop_count_int++;
    } while (*value_ptr != 0 && loop_count_int < 1000000);

    assign_ret_ref(value_ptr);
}

prjm_eel_function_decl(if)
{
    assert_valid_ctx();

    float* if_arg = &ctx->value;

    invoke_arg(1, &if_arg);

    if ((*if_arg) != 0)
    {
        invoke_arg(2, ret_val);
        return;
    }
    invoke_arg(3, ret_val);
}

prjm_eel_function_decl(exec2)
{
    assert_valid_ctx();

    ctx->value = .0f;
    float* value_ptr = &ctx->value;

    invoke_arg(1, &value_ptr);
    invoke_arg(2, ret_val);
}

prjm_eel_function_decl(exec3)
{
    assert_valid_ctx();

    ctx->value = .0f;
    float* value_ptr = &ctx->value;

    invoke_arg(1, &value_ptr);
    invoke_arg(2, &value_ptr);
    invoke_arg(3, ret_val);
}

prjm_eel_function_decl(set)
{
    assert_valid_ctx();

    ctx->value = .0f;
    float* value_ptr = &ctx->value;

    invoke_arg(1, ret_val);
    invoke_arg(2, &value_ptr);

    assign_ret_val(*value_ptr);
}


/* Memory access functions */
prjm_eel_function_decl(megabuf)
{
    assert_valid_ctx();

}

prjm_eel_function_decl(gmegabuf)
{
    assert_valid_ctx();

}

prjm_eel_function_decl(freembuf)
{
    assert_valid_ctx();

}

prjm_eel_function_decl(memcpy)
{
    assert_valid_ctx();

}

prjm_eel_function_decl(memset)
{
    assert_valid_ctx();

}



/* Operators */

prjm_eel_function_decl(bnot)
{
    assert_valid_ctx();

    ctx->value = .0f;
    float* value_ptr = &ctx->value;

    invoke_arg(1, &value_ptr);

    assign_ret_val((float) !(*value_ptr));
}

prjm_eel_function_decl(equal)
{
    assert_valid_ctx();

    float val1 = .0f;
    float val2 = .0f;
    float* val1_ptr = &val1;
    float* val2_ptr = &val2;

    invoke_arg(1, &val1_ptr);
    invoke_arg(2, &val2_ptr);

    assign_ret_val((*val1_ptr == *val2_ptr) ? 1.f : .0f);
}

prjm_eel_function_decl(notequal)
{
    assert_valid_ctx();

    float val1 = .0f;
    float val2 = .0f;
    float* val1_ptr = &val1;
    float* val2_ptr = &val2;
    invoke_arg(1, &val1_ptr);
    invoke_arg(2, &val2_ptr);

    assign_ret_val((*val1_ptr != *val2_ptr) ? 1.f : .0f);
}

prjm_eel_function_decl(below)
{
    assert_valid_ctx();

    float val1 = .0f;
    float val2 = .0f;
    float* val1_ptr = &val1;
    float* val2_ptr = &val2;

    invoke_arg(1, &val1_ptr);
    invoke_arg(2, &val2_ptr);

    assign_ret_val((*val1_ptr < *val2_ptr) ? 1.f : .0f);
}

prjm_eel_function_decl(above)
{
    assert_valid_ctx();

    float val1 = .0f;
    float val2 = .0f;
    float* val1_ptr = &val1;
    float* val2_ptr = &val2;

    invoke_arg(1, &val1_ptr);
    invoke_arg(2, &val2_ptr);

    assign_ret_val((*val1_ptr > *val2_ptr) ? 1.f : .0f);
}

prjm_eel_function_decl(beloweq)
{
    assert_valid_ctx();

    float val1 = .0f;
    float val2 = .0f;
    float* val1_ptr = &val1;
    float* val2_ptr = &val2;

    invoke_arg(1, &val1_ptr);
    invoke_arg(2, &val2_ptr);

    assign_ret_val((*val1_ptr <= *val2_ptr) ? 1.f : .0f);
}

prjm_eel_function_decl(aboveeq)
{
    assert_valid_ctx();

    float val1 = .0f;
    float val2 = .0f;
    float* val1_ptr = &val1;
    float* val2_ptr = &val2;

    invoke_arg(1, &val1_ptr);
    invoke_arg(2, &val2_ptr);

    assign_ret_val((*val1_ptr >= *val2_ptr) ? 1.f : .0f);
}

prjm_eel_function_decl(add)
{
    assert_valid_ctx();

    float val1 = .0f;
    float val2 = .0f;
    float* val1_ptr = &val1;
    float* val2_ptr = &val2;

    invoke_arg(1, &val1_ptr);
    invoke_arg(2, &val2_ptr);

    assign_ret_val(*val1_ptr + *val2_ptr);
}

prjm_eel_function_decl(sub)
{
    assert_valid_ctx();

    float val1 = .0f;
    float val2 = .0f;
    float* val1_ptr = &val1;
    float* val2_ptr = &val2;

    invoke_arg(1, &val1_ptr);
    invoke_arg(2, &val2_ptr);

    assign_ret_val(*val1_ptr - *val2_ptr);
}

prjm_eel_function_decl(mul)
{
    assert_valid_ctx();

    float val1 = .0f;
    float val2 = .0f;
    float* val1_ptr = &val1;
    float* val2_ptr = &val2;

    invoke_arg(1, &val1_ptr);
    invoke_arg(2, &val2_ptr);

    assign_ret_val(*val1_ptr * *val2_ptr);
}

prjm_eel_function_decl(div)
{
    assert_valid_ctx();

    float val1 = .0f;
    float val2 = .0f;
    float* val1_ptr = &val1;
    float* val2_ptr = &val2;

    invoke_arg(1, &val1_ptr);
    invoke_arg(2, &val2_ptr);

    assign_ret_val(*val1_ptr / *val2_ptr);
}

prjm_eel_function_decl(mod)
{
    assert_valid_ctx();

    float val1 = .0f;
    float val2 = .0f;
    float* val1_ptr = &val1;
    float* val2_ptr = &val2;

    invoke_arg(1, &val1_ptr);
    invoke_arg(2, &val2_ptr);

    assign_ret_val((float) ((int) *val1_ptr % (int) *val2_ptr));
}

prjm_eel_function_decl(band)
{
    assert_valid_ctx();

    float val1 = .0f;
    float val2 = .0f;
    float* val1_ptr = &val1;
    float* val2_ptr = &val2;

    invoke_arg(1, &val1_ptr);
    invoke_arg(2, &val2_ptr);

    assign_ret_val((float) ((int) *val1_ptr & (int) *val2_ptr));
}

prjm_eel_function_decl(bor)
{
    assert_valid_ctx();

    float val1 = .0f;
    float val2 = .0f;
    float* val1_ptr = &val1;
    float* val2_ptr = &val2;

    invoke_arg(1, &val1_ptr);
    invoke_arg(2, &val2_ptr);

    assign_ret_val((float) ((int) *val1_ptr | (int) *val2_ptr));
}


prjm_eel_function_decl(addop)
{
    assert_valid_ctx();

    float val2 = .0f;
    float* val1_ptr = &ctx->value;
    float* val2_ptr = &val2;

    invoke_arg(1, &val1_ptr);
    invoke_arg(2, &val2_ptr);

    (*val1_ptr) = *val1_ptr + *val2_ptr;
    assign_ret_ref(val1_ptr);
}

prjm_eel_function_decl(subop)
{
    assert_valid_ctx();

    float val2 = .0f;
    float* val1_ptr = &ctx->value;
    float* val2_ptr = &val2;

    invoke_arg(1, &val1_ptr);
    invoke_arg(2, &val2_ptr);

    (*val1_ptr) = *val1_ptr - *val2_ptr;
    assign_ret_ref(val1_ptr);
}

prjm_eel_function_decl(mulop)
{
    assert_valid_ctx();

    float val2 = .0f;
    float* val1_ptr = &ctx->value;
    float* val2_ptr = &val2;

    invoke_arg(1, &val1_ptr);
    invoke_arg(2, &val2_ptr);

    (*val1_ptr) = *val1_ptr * *val2_ptr;
    assign_ret_ref(val1_ptr);
}

prjm_eel_function_decl(divop)
{
    assert_valid_ctx();

    float val2 = .0f;
    float* val1_ptr = &ctx->value;
    float* val2_ptr = &val2;

    invoke_arg(1, &val1_ptr);
    invoke_arg(2, &val2_ptr);

    (*val1_ptr) = *val1_ptr / *val2_ptr;
    assign_ret_ref(val1_ptr);
}

prjm_eel_function_decl(orop)
{
    assert_valid_ctx();

    float val2 = .0f;
    float* val1_ptr = &ctx->value;
    float* val2_ptr = &val2;

    invoke_arg(1, &val1_ptr);
    invoke_arg(2, &val2_ptr);

    (*val1_ptr) = (float) ((int) *val1_ptr & (int) *val2_ptr);
    assign_ret_ref(val1_ptr);
}

prjm_eel_function_decl(andop)
{
    assert_valid_ctx();

    float val2 = .0f;
    float* val1_ptr = &ctx->value;
    float* val2_ptr = &val2;

    invoke_arg(1, &val1_ptr);
    invoke_arg(2, &val2_ptr);

    (*val1_ptr) = (float) ((int) *val1_ptr | (int) *val2_ptr);
    assign_ret_ref(val1_ptr);
}

prjm_eel_function_decl(modop)
{
    assert_valid_ctx();

    float val2 = .0f;
    float* val1_ptr = &ctx->value;
    float* val2_ptr = &val2;

    invoke_arg(1, &val1_ptr);
    invoke_arg(2, &val2_ptr);

    (*val1_ptr) = (float) ((int) (*val1_ptr) % (int) (*val2_ptr));
    assign_ret_ref(val1_ptr);
}

prjm_eel_function_decl(powop)
{
    assert_valid_ctx();

    float val2 = .0f;
    float* val1_ptr = &ctx->value;
    float* val2_ptr = &val2;

    invoke_arg(1, &val1_ptr);
    invoke_arg(2, &val2_ptr);

    (*val1_ptr) = powf(*val1_ptr, *val2_ptr);
    assign_ret_ref(val1_ptr);
}


/* Math functions */
prjm_eel_function_decl(math_func1)
{
    assert_valid_ctx();
    assert(ctx->math_func);

    ctx->value = .0f;
    float* math_arg_ptr = &ctx->value;

    invoke_arg(1, &math_arg_ptr);

    assign_ret_val(((prjm_eel_math_func1*) ctx->math_func)(*math_arg_ptr));
}

prjm_eel_function_decl(math_func2)
{
    assert_valid_ctx();
    assert(ctx->math_func);

    float math_arg1 = .0f;
    float math_arg2 = .0f;
    float* math_arg1_ptr = &math_arg1;
    float* math_arg2_ptr = &math_arg2;

    invoke_arg(1, &math_arg1_ptr);
    invoke_arg(2, &math_arg2_ptr);

    assign_ret_val(((prjm_eel_math_func2*) ctx->math_func)(*math_arg1_ptr, *math_arg2_ptr));
}

prjm_eel_function_decl(sigmoid)
{
    assert_valid_ctx();

    float math_arg1 = .0f;
    float math_arg2 = .0f;
    float* math_arg1_ptr = &math_arg1;
    float* math_arg2_ptr = &math_arg2;

    invoke_arg(1, &math_arg1_ptr);
    invoke_arg(2, &math_arg2_ptr);

    double t = (1 + exp((double) -(*math_arg1_ptr) * (*math_arg2_ptr)));
    assign_ret_val((float) (fabs(t) > 0.00001 ? 1.0 / t : 0.f));
}

prjm_eel_function_decl(sqr)
{
    assert_valid_ctx();

    ctx->value = .0f;
    float* value_ptr = &ctx->value;

    invoke_arg(1, &value_ptr);

    assign_ret_val((*value_ptr) * (*value_ptr));
}

prjm_eel_function_decl(abs)
{
    assert_valid_ctx();

    ctx->value = .0f;
    float* value_ptr = &ctx->value;

    invoke_arg(1, &value_ptr);

    assign_ret_val(fabsf(*value_ptr));
}

prjm_eel_function_decl(min)
{
    assert_valid_ctx();

    float math_arg1 = .0f;
    float math_arg2 = .0f;
    float* math_arg1_ptr = &math_arg1;
    float* math_arg2_ptr = &math_arg2;

    invoke_arg(1, &math_arg1_ptr);
    invoke_arg(2, &math_arg2_ptr);

    assign_ret_val((*math_arg1_ptr) < (*math_arg2_ptr) ? (*math_arg1_ptr) : (*math_arg2_ptr));
}

prjm_eel_function_decl(max)
{
    assert_valid_ctx();

    float math_arg1 = .0f;
    float math_arg2 = .0f;
    float* math_arg1_ptr = &math_arg1;
    float* math_arg2_ptr = &math_arg2;

    invoke_arg(1, &math_arg1_ptr);
    invoke_arg(2, &math_arg2_ptr);

    assign_ret_val((*math_arg1_ptr) > (*math_arg2_ptr) ? (*math_arg1_ptr) : (*math_arg2_ptr));
}

prjm_eel_function_decl(sign)
{
    assert_valid_ctx();

    ctx->value = .0f;
    float* value_ptr = &ctx->value;

    invoke_arg(1, &value_ptr);

    if (*value_ptr == 0)
    {
        (**ret_val) = .0f;
        return;
    }
    assign_ret_val((*value_ptr) < .0f ? -1.f : 1.f);
}

prjm_eel_function_decl(rand)
{
    assert_valid_ctx();

    ctx->value = .0f;
    float* value_ptr = &ctx->value;

    invoke_arg(1, &value_ptr);

    float rand_max = floorf(*value_ptr);
    if (rand_max < 1.f)
    {
        rand_max = 1.f;
    }

    assign_ret_val((float) (prjm_eel_genrand_int32() * (1.0 / (double) 0xFFFFFFFF * rand_max)));
}

prjm_eel_function_decl(invsqrt)
{
    assert_valid_ctx();

    /* Using fast inverse square root implementation here, same as Milkdrop.
     * See https://en.wikipedia.org/wiki/Fast_inverse_square_root
     */
    union
    {
        float float_val;
        uint32_t int_val;
    } type_conv;

    static const float three_halfs = 1.5f;
    static const float one_half = .5f;

    ctx->value = .0f;
    float* value_ptr = &ctx->value;

    invoke_arg(1, &value_ptr);

    float num2 = (*value_ptr) * one_half;
    type_conv.float_val = (*value_ptr);
    type_conv.int_val = 0x5f3759df - (type_conv.int_val >> 1);
    type_conv.float_val = type_conv.float_val * (three_halfs - (num2 * type_conv.float_val * type_conv.float_val));

    assign_ret_val(type_conv.float_val);
}