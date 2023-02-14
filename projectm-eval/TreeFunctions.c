/**
 * @file TreeFunctions.c
 * @brief Implements all intrinsic projectM-EvalLib functions and helpers.
 */
#include "TreeFunctions.h"

#include "MemoryBuffer.h"

#include <math.h>
#include <assert.h>
#include <stdint.h>

#ifdef _MSC_VER
#pragma function (floor)
#pragma function (ceil)
#endif

/**
 * @brief projectM-EvalLib intrinsic Function table.
 * Contains all predefined functions and information about their invocation. Most functions beginning
 * with _ are not really required because they're internals of ns-eel2, but as they can be used in
 * Milkdrop, we also allow expressions to call them directly.
 */
static prjm_eel_function_def_t intrinsic_function_table[] = {
    /* Special intrinsic functions. Cannot be used via expression syntax. */
    { "/*const*/", prjm_eel_func_const,         0,     0, true,  false },
    { "/*var*/",   prjm_eel_func_var,           0,     0, false, false },
    { "/*list*/",  prjm_eel_func_execute_list,  0,     1, true,  false },

    { "if",        prjm_eel_func_if,            0,     3, true,  false },
    { "_if",       prjm_eel_func_if,            0,     3, true,  false },
    { "_and",      prjm_eel_func_band_op,       0,     2, true,  false },
    { "_or",       prjm_eel_func_bor_op,        0,     2, true,  false },
    { "loop",      prjm_eel_func_execute_loop,  0,     2, true,  false },
    { "while",     prjm_eel_func_execute_while, 0,     1, true,  false },

    { "_not",      prjm_eel_func_bnot,          0,     1, true,  false },
    { "bnot",      prjm_eel_func_bnot,          0,     1, true,  false },
    { "_equal",    prjm_eel_func_equal,         0,     2, true,  false },
    { "equal",     prjm_eel_func_equal,         0,     2, true,  false },
    { "_noteq",    prjm_eel_func_notequal,      0,     2, true,  false },
    { "_below",    prjm_eel_func_below,         0,     2, true,  false },
    { "below",     prjm_eel_func_below,         0,     2, true,  false },
    { "_above",    prjm_eel_func_above,         0,     2, true,  false },
    { "above",     prjm_eel_func_above,         0,     2, true,  false },
    { "_beleq",    prjm_eel_func_beloweq,       0,     2, true,  false },
    { "_aboeq",    prjm_eel_func_aboveeq,       0,     2, true,  false },

    { "_set",      prjm_eel_func_set,           0,     2, false, true },
    { "assign",    prjm_eel_func_set,           0,     2, false, true },
    { "_add",      prjm_eel_func_add,           0,     2, true,  false },
    { "_sub",      prjm_eel_func_sub,           0,     2, true,  false },
    { "_mul",      prjm_eel_func_mul,           0,     2, true,  false },
    { "_div",      prjm_eel_func_div,           0,     2, true,  false },
    { "_mod",      prjm_eel_func_mod,           0,     2, true,  false },
    { "_mulop",    prjm_eel_func_mulop,         0,     2, false, true },
    { "_divop",    prjm_eel_func_divop,         0,     2, false, true },
    { "_orop",     prjm_eel_func_orop,          0,     2, false, true },
    { "_andop",    prjm_eel_func_andop,         0,     2, false, true },
    { "_addop",    prjm_eel_func_addop,         0,     2, false, true },
    { "_subop",    prjm_eel_func_subop,         0,     2, false, true },
    { "_modop",    prjm_eel_func_modop,         0,     2, false, true },

    { "sin",       prjm_eel_func_math_func1,    sin,   1, true,  false },
    { "cos",       prjm_eel_func_math_func1,    cos,   1, true,  false },
    { "tan",       prjm_eel_func_math_func1,    tan,   1, true,  false },
    { "asin",      prjm_eel_func_math_func1,    asin,  1, true,  false },
    { "acos",      prjm_eel_func_math_func1,    acos,  1, true,  false },
    { "atan",      prjm_eel_func_math_func1,    atan,  1, true,  false },
    { "atan2",     prjm_eel_func_math_func2,    atan2, 2, true,  false },
    { "sqr",       prjm_eel_func_sqr,           0,     1, true,  false },
    { "sqrt",      prjm_eel_func_math_func1,    sqrt,  1, true,  false },
    { "pow",       prjm_eel_func_math_func2,    pow,   2, true,  false },
    { "_powop",    prjm_eel_func_powop,         0,     2, false, true },
    { "exp",       prjm_eel_func_math_func1,    exp,   1, true,  false },
    { "_neg",      prjm_eel_func_neg,           0,     1, true,  false },

    { "log",       prjm_eel_func_math_func1,    log,   1, true,  false },
    { "log10",     prjm_eel_func_math_func1,    log10, 1, true,  false },
    { "abs",       prjm_eel_func_abs,           0,     1, true,  false },
    { "min",       prjm_eel_func_min,           0,     2, true,  false },
    { "max",       prjm_eel_func_max,           0,     2, true,  false },
    { "sign",      prjm_eel_func_sign,          0,     1, true,  false },
    { "rand",      prjm_eel_func_rand,          0,     1, false, false },
    { "floor",     prjm_eel_func_math_func1,    floor, 1, true,  false },
    { "int",       prjm_eel_func_math_func1,    floor, 1, true,  false },
    { "ceil",      prjm_eel_func_math_func1,    ceil,  1, true,  false },
    { "invsqrt",   prjm_eel_func_invsqrt,       0,     1, true,  false },
    { "sigmoid",   prjm_eel_func_sigmoid,       0,     2, true,  false },

    { "band",      prjm_eel_func_band_func,     0,     2, true,  false },
    { "bor",       prjm_eel_func_bor_func,      0,     2, true,  false },

    { "exec2",     prjm_eel_func_exec2,         0,     2, true,  false },
    { "exec3",     prjm_eel_func_exec3,         0,     3, true,  false },
    { "_mem",      prjm_eel_func_mem,           0,     1, false, true },
    { "megabuf",   prjm_eel_func_mem,           0,     1, false, true },
    { "_gmem",     prjm_eel_func_mem,           0,     1, false, true },
    { "gmegabuf",  prjm_eel_func_mem,           0,     1, false, true },
    { "freembuf",  prjm_eel_func_freembuf,      0,     1, false, true },
    { "memcpy",    prjm_eel_func_memcpy,        0,     3, false, true },
    { "memset",    prjm_eel_func_memset,        0,     3, false, true }
};


/* Helper functions and defines */
#define assert_valid_ctx() \
        assert(ctx); \
        assert(ret_val); \
        assert(*ret_val); \
        assert(ctx->func)

/* Allowed error for float/double comparisons to exact values */
#define COMPARE_CLOSEFACTOR 0.00001
static const PRJM_EVAL_F close_factor = COMPARE_CLOSEFACTOR;

/* These factors are not exactly as close to zero as their ns-eel2 equivalents, but that shouldn't
 * matter too much. In ns-eel2, the value is represented as binary 0x00000000FFFFFFFF for doubles.
 */
#if PRJM_F_SIZE == 4
static const PRJM_EVAL_F close_factor_low = 1e-41;
#define LRINT lrintf
#else
static const PRJM_EVAL_F close_factor_low = 1e-300;
#define LRINT lrint
#endif

/* Maximum number of loop iterations */
#define MAX_LOOP_COUNT 1048576

/**
 * Invokes the expression function of the indexed argument
 */
#define invoke_arg(argnum, retval) \
    assert(ctx->args[argnum]);    \
    ctx->args[argnum]->func(ctx->args[argnum], retval)

/**
 * Assigns the value as a constant PRJM_F return value.
 * @param value The expression or value to assign. Must evaluate to PRJM_F.
 */
#define assign_ret_val(value) \
    (**ret_val) = value

/**
 * Assigns the value as a constant PRJM_F return value.
 * @param ref The reference pointer to assign. Must evaluate to PRJM_F*.
 */
#define assign_ret_ref(ref) \
    (*ret_val) = ref

/* Used in genrand_int32 */
#define N 624
#define M 397
#define MATRIX_A 0x9908b0dfUL   /* constant vector a */
#define UPPER_MASK 0x80000000UL /* most significant w-r bits */
#define LOWER_MASK 0x7fffffffUL /* least significant r bits */

void prjm_eel_intrinsic_functions(prjm_eel_intrinsic_function_list_ptr list, int* count)
{
    *count = sizeof(intrinsic_function_table) / sizeof(prjm_eel_function_def_t);
    *list = intrinsic_function_table;
}

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

    assign_ret_ref(ctx->var);
}


/* Execution control structures */
prjm_eel_function_decl(execute_list)
{
    assert_valid_ctx();
    assert(ctx->list);

    ctx->value = .0;
    PRJM_EVAL_F* value_ptr = &ctx->value;
    prjm_eel_exptreenode_list_item_t* item = ctx->list;
    while (item)
    {
        assert(item->expr);
        assert(item->expr->func);

        ctx->value = .0;
        value_ptr = &ctx->value;
        item->expr->func(item->expr, &value_ptr);
        item = item->next;
    }

    assign_ret_ref(value_ptr);
}

prjm_eel_function_decl(execute_loop)
{
    assert_valid_ctx();

    ctx->value = .0;
    PRJM_EVAL_F* value_ptr = &ctx->value;
    invoke_arg(0, &value_ptr);

    int loop_count_int = (int) (*value_ptr);
    /* Limit execution count */
    if (loop_count_int > MAX_LOOP_COUNT)
    {
        loop_count_int = MAX_LOOP_COUNT;
    }

    for (int i = 0; i < loop_count_int; i++)
    {
        ctx->value = .0;
        value_ptr = &ctx->value;
        invoke_arg(1, &value_ptr);
    }

    assign_ret_ref(value_ptr);
}

prjm_eel_function_decl(execute_while)
{
    assert_valid_ctx();

    ctx->value = .0;
    PRJM_EVAL_F* value_ptr = &ctx->value;
    int loop_count_int = MAX_LOOP_COUNT;
    do
    {
        invoke_arg(0, &value_ptr);
    } while (fabs(*value_ptr) > close_factor_low && --loop_count_int);

    assign_ret_ref(value_ptr);
}

prjm_eel_function_decl(if)
{
    assert_valid_ctx();

    PRJM_EVAL_F* if_arg = &ctx->value;

    invoke_arg(0, &if_arg);

    if ((*if_arg) != 0)
    {
        invoke_arg(1, ret_val);
        return;
    }
    invoke_arg(2, ret_val);
}

prjm_eel_function_decl(exec2)
{
    assert_valid_ctx();

    ctx->value = .0;
    PRJM_EVAL_F* value_ptr = &ctx->value;

    invoke_arg(0, &value_ptr);
    invoke_arg(1, ret_val);
}

prjm_eel_function_decl(exec3)
{
    assert_valid_ctx();

    ctx->value = .0;
    PRJM_EVAL_F* value_ptr = &ctx->value;

    invoke_arg(0, &value_ptr);
    invoke_arg(1, &value_ptr);
    invoke_arg(2, ret_val);
}

prjm_eel_function_decl(set)
{
    assert_valid_ctx();

    ctx->value = .0;
    PRJM_EVAL_F* value_ptr = &ctx->value;

    invoke_arg(0, ret_val);
    invoke_arg(1, &value_ptr);

    assign_ret_val(*value_ptr);
}


/* Memory access functions */
prjm_eel_function_decl(mem)
{
    assert_valid_ctx();
    assert(ctx->memory_buffer);

    ctx->value = .0;
    PRJM_EVAL_F* index_ptr = &ctx->value;
    invoke_arg(0, &index_ptr);

    PRJM_EVAL_F* mem_addr = prjm_eel_memory_allocate(ctx->memory_buffer, (int) lrint(*index_ptr + 0.0001));
    if (mem_addr)
    {
        assign_ret_ref(mem_addr);
        return;
    }

    assign_ret_val(.0);
}

prjm_eel_function_decl(freembuf)
{
    assert_valid_ctx();
    assert(ctx->memory_buffer);

    invoke_arg(0, ret_val);

    prjm_eel_memory_free(ctx->memory_buffer);
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

    ctx->value = .0;
    PRJM_EVAL_F* value_ptr = &ctx->value;

    invoke_arg(0, &value_ptr);

    assign_ret_val(fabs(*value_ptr) < close_factor_low ? 1.0 : 0.0);
}

prjm_eel_function_decl(equal)
{
    assert_valid_ctx();

    PRJM_EVAL_F val1 = .0;
    PRJM_EVAL_F val2 = .0;
    PRJM_EVAL_F* val1_ptr = &val1;
    PRJM_EVAL_F* val2_ptr = &val2;

    invoke_arg(0, &val1_ptr);
    invoke_arg(1, &val2_ptr);

    assign_ret_val(fabs(*val1_ptr - *val2_ptr) < close_factor_low ? 1.0 : 0.0);
}

prjm_eel_function_decl(notequal)
{
    assert_valid_ctx();

    PRJM_EVAL_F val1 = .0;
    PRJM_EVAL_F val2 = .0;
    PRJM_EVAL_F* val1_ptr = &val1;
    PRJM_EVAL_F* val2_ptr = &val2;
    invoke_arg(0, &val1_ptr);
    invoke_arg(1, &val2_ptr);

    assign_ret_val(fabs(*val1_ptr - *val2_ptr) > close_factor_low ? 1.0 : 0.0);
}

prjm_eel_function_decl(below)
{
    assert_valid_ctx();

    PRJM_EVAL_F val1 = .0;
    PRJM_EVAL_F val2 = .0;
    PRJM_EVAL_F* val1_ptr = &val1;
    PRJM_EVAL_F* val2_ptr = &val2;

    invoke_arg(0, &val1_ptr);
    invoke_arg(1, &val2_ptr);

    assign_ret_val((*val1_ptr < *val2_ptr) ? 1.0 : 0.0);
}

prjm_eel_function_decl(above)
{
    assert_valid_ctx();

    PRJM_EVAL_F val1 = .0;
    PRJM_EVAL_F val2 = .0;
    PRJM_EVAL_F* val1_ptr = &val1;
    PRJM_EVAL_F* val2_ptr = &val2;

    invoke_arg(0, &val1_ptr);
    invoke_arg(1, &val2_ptr);

    assign_ret_val((*val1_ptr > *val2_ptr) ? 1.0 : 0.0);
}

prjm_eel_function_decl(beloweq)
{
    assert_valid_ctx();

    PRJM_EVAL_F val1 = .0;
    PRJM_EVAL_F val2 = .0;
    PRJM_EVAL_F* val1_ptr = &val1;
    PRJM_EVAL_F* val2_ptr = &val2;

    invoke_arg(0, &val1_ptr);
    invoke_arg(1, &val2_ptr);

    assign_ret_val((*val1_ptr <= *val2_ptr) ? 1.0 : 0.0);
}

prjm_eel_function_decl(aboveeq)
{
    assert_valid_ctx();

    PRJM_EVAL_F val1 = .0;
    PRJM_EVAL_F val2 = .0;
    PRJM_EVAL_F* val1_ptr = &val1;
    PRJM_EVAL_F* val2_ptr = &val2;

    invoke_arg(0, &val1_ptr);
    invoke_arg(1, &val2_ptr);

    assign_ret_val((*val1_ptr >= *val2_ptr) ? 1.0 : 0.0);
}

prjm_eel_function_decl(add)
{
    assert_valid_ctx();

    PRJM_EVAL_F val1 = .0;
    PRJM_EVAL_F val2 = .0;
    PRJM_EVAL_F* val1_ptr = &val1;
    PRJM_EVAL_F* val2_ptr = &val2;

    invoke_arg(0, &val1_ptr);
    invoke_arg(1, &val2_ptr);

    assign_ret_val(*val1_ptr + *val2_ptr);
}

prjm_eel_function_decl(sub)
{
    assert_valid_ctx();

    PRJM_EVAL_F val1 = .0;
    PRJM_EVAL_F val2 = .0;
    PRJM_EVAL_F* val1_ptr = &val1;
    PRJM_EVAL_F* val2_ptr = &val2;

    invoke_arg(0, &val1_ptr);
    invoke_arg(1, &val2_ptr);

    assign_ret_val(*val1_ptr - *val2_ptr);
}

prjm_eel_function_decl(mul)
{
    assert_valid_ctx();

    PRJM_EVAL_F val1 = .0;
    PRJM_EVAL_F val2 = .0;
    PRJM_EVAL_F* val1_ptr = &val1;
    PRJM_EVAL_F* val2_ptr = &val2;

    invoke_arg(0, &val1_ptr);
    invoke_arg(1, &val2_ptr);

    assign_ret_val(*val1_ptr * *val2_ptr);
}

prjm_eel_function_decl(div)
{
    assert_valid_ctx();

    PRJM_EVAL_F val1 = .0;
    PRJM_EVAL_F val2 = .0;
    PRJM_EVAL_F* val1_ptr = &val1;
    PRJM_EVAL_F* val2_ptr = &val2;

    invoke_arg(0, &val1_ptr);
    invoke_arg(1, &val2_ptr);

    assign_ret_val(*val1_ptr / *val2_ptr);
}

prjm_eel_function_decl(mod)
{
    assert_valid_ctx();

    PRJM_EVAL_F val1 = .0;
    PRJM_EVAL_F val2 = .0;
    PRJM_EVAL_F* val1_ptr = &val1;
    PRJM_EVAL_F* val2_ptr = &val2;

    invoke_arg(0, &val1_ptr);
    invoke_arg(1, &val2_ptr);

    int divisor = (int) *val2_ptr;
    if (divisor == 0)
    {
        assign_ret_val(0.0);
        return;
    }
    assign_ret_val((PRJM_EVAL_F) ((int) *val1_ptr % divisor));
}

prjm_eel_function_decl(band_op)
{
    assert_valid_ctx();

    PRJM_EVAL_F val1 = .0;
    PRJM_EVAL_F* val1_ptr = &val1;
    PRJM_EVAL_F val2 = .0;
    PRJM_EVAL_F* val2_ptr = &val2;

    /*
     * The && operator differs from the "band" function as it only evaluates the second
     * argument if the first one is non-zero.
     */
    invoke_arg(0, &val1_ptr);

    if (fabs(*val1_ptr) > close_factor_low)
    {
        invoke_arg(1, &val2_ptr);

        assign_ret_val(fabs(*val2_ptr) > close_factor_low ? 1.0 : 0.0);
    }
    else
    {
        assign_ret_val(0.0);
    }
}

prjm_eel_function_decl(bor_op)
{
    assert_valid_ctx();

    PRJM_EVAL_F val1 = .0;
    PRJM_EVAL_F* val1_ptr = &val1;
    PRJM_EVAL_F val2 = .0;
    PRJM_EVAL_F* val2_ptr = &val2;

    /*
     * The || operator differs from the "bor" function as it only evaluates the second
     * argument if the first one is zero.
     */
    invoke_arg(0, &val1_ptr);

    if (fabs(*val1_ptr) < close_factor_low)
    {
        invoke_arg(1, &val2_ptr);

        assign_ret_val(fabs(*val2_ptr) > close_factor_low ? 1.0 : 0.0);
    }
    else
    {
        assign_ret_val(1.0);
    }
}

prjm_eel_function_decl(band_func)
{
    assert_valid_ctx();

    PRJM_EVAL_F val1 = .0;
    PRJM_EVAL_F val2 = .0;
    PRJM_EVAL_F* val1_ptr = &val1;
    PRJM_EVAL_F* val2_ptr = &val2;

    invoke_arg(0, &val1_ptr);
    invoke_arg(1, &val2_ptr);

    /* This function also uses the larger close factor! */
    assign_ret_val(fabs(*val1_ptr) > close_factor && fabs(*val2_ptr) > close_factor ? 1.0 : 0.0);
}

prjm_eel_function_decl(bor_func)
{
    assert_valid_ctx();

    PRJM_EVAL_F val1 = .0;
    PRJM_EVAL_F val2 = .0;
    PRJM_EVAL_F* val1_ptr = &val1;
    PRJM_EVAL_F* val2_ptr = &val2;

    invoke_arg(0, &val1_ptr);
    invoke_arg(1, &val2_ptr);

    /* This function also uses the larger close factor! */
    assign_ret_val(fabs(*val1_ptr) > close_factor || fabs(*val2_ptr) > close_factor ? 1.0 : 0.0);
}

prjm_eel_function_decl(neg)
{
    assert_valid_ctx();

    PRJM_EVAL_F val1 = .0;
    PRJM_EVAL_F* val1_ptr = &val1;

    invoke_arg(0, &val1_ptr);

    assign_ret_val(-(*val1_ptr));
}

prjm_eel_function_decl(addop)
{
    assert_valid_ctx();

    PRJM_EVAL_F val2 = .0;
    PRJM_EVAL_F* val2_ptr = &val2;

    invoke_arg(0, ret_val);
    invoke_arg(1, &val2_ptr);

    assign_ret_val(**ret_val + *val2_ptr);
}

prjm_eel_function_decl(subop)
{
    assert_valid_ctx();

    PRJM_EVAL_F val2 = .0;
    PRJM_EVAL_F* val2_ptr = &val2;

    invoke_arg(0, ret_val);
    invoke_arg(1, &val2_ptr);

    assign_ret_val(**ret_val - *val2_ptr);
}

prjm_eel_function_decl(mulop)
{
    assert_valid_ctx();

    PRJM_EVAL_F val2 = .0;
    PRJM_EVAL_F* val2_ptr = &val2;

    invoke_arg(0, ret_val);
    invoke_arg(1, &val2_ptr);

    assign_ret_val(**ret_val * *val2_ptr);
}

prjm_eel_function_decl(divop)
{
    assert_valid_ctx();

    PRJM_EVAL_F val2 = .0;
    PRJM_EVAL_F* val2_ptr = &val2;

    invoke_arg(0, ret_val);
    invoke_arg(1, &val2_ptr);

    assign_ret_val(**ret_val / *val2_ptr);
}

prjm_eel_function_decl(orop)
{
    assert_valid_ctx();

    PRJM_EVAL_F val2 = .0;
    PRJM_EVAL_F* val2_ptr = &val2;

    invoke_arg(0, ret_val);
    invoke_arg(1, &val2_ptr);

    assign_ret_val((PRJM_EVAL_F) (LRINT(**ret_val) | LRINT(*val2_ptr)));
}

prjm_eel_function_decl(andop)
{
    assert_valid_ctx();

    PRJM_EVAL_F val2 = .0;
    PRJM_EVAL_F* val2_ptr = &val2;

    invoke_arg(0, ret_val);
    invoke_arg(1, &val2_ptr);

    assign_ret_val((PRJM_EVAL_F) (LRINT(**ret_val) & LRINT(*val2_ptr)));
}

prjm_eel_function_decl(modop)
{
    assert_valid_ctx();

    PRJM_EVAL_F val2 = .0;
    PRJM_EVAL_F* val2_ptr = &val2;

    invoke_arg(0, ret_val);
    invoke_arg(1, &val2_ptr);

    assign_ret_val((PRJM_EVAL_F) (LRINT(**ret_val) % LRINT(*val2_ptr)));
}

prjm_eel_function_decl(powop)
{
    assert_valid_ctx();

    PRJM_EVAL_F val2 = .0;
    PRJM_EVAL_F* val2_ptr = &val2;

    invoke_arg(0, ret_val);
    invoke_arg(1, &val2_ptr);

    assign_ret_val(pow(**ret_val, *val2_ptr));
}


/* Math functions */
prjm_eel_function_decl(math_func1)
{
    assert_valid_ctx();
    assert(ctx->math_func);

    ctx->value = .0;
    PRJM_EVAL_F* math_arg_ptr = &ctx->value;

    invoke_arg(0, &math_arg_ptr);

    assign_ret_val(((prjm_eel_math_func1*) ctx->math_func)(*math_arg_ptr));
}

prjm_eel_function_decl(math_func2)
{
    assert_valid_ctx();
    assert(ctx->math_func);

    PRJM_EVAL_F math_arg1 = .0;
    PRJM_EVAL_F math_arg2 = .0;
    PRJM_EVAL_F* math_arg1_ptr = &math_arg1;
    PRJM_EVAL_F* math_arg2_ptr = &math_arg2;

    invoke_arg(0, &math_arg1_ptr);
    invoke_arg(1, &math_arg2_ptr);

    assign_ret_val(((prjm_eel_math_func2*) ctx->math_func)(*math_arg1_ptr, *math_arg2_ptr));
}

prjm_eel_function_decl(sigmoid)
{
    assert_valid_ctx();

    PRJM_EVAL_F math_arg1 = .0;
    PRJM_EVAL_F math_arg2 = .0;
    PRJM_EVAL_F* math_arg1_ptr = &math_arg1;
    PRJM_EVAL_F* math_arg2_ptr = &math_arg2;

    invoke_arg(0, &math_arg1_ptr);
    invoke_arg(1, &math_arg2_ptr);

    double t = (1 + exp((double) -(*math_arg1_ptr) * (*math_arg2_ptr)));
    assign_ret_val((PRJM_EVAL_F) (fabs(t) > close_factor ? 1.0 / t : .0));
}

prjm_eel_function_decl(sqr)
{
    assert_valid_ctx();

    ctx->value = .0;
    PRJM_EVAL_F* value_ptr = &ctx->value;

    invoke_arg(0, &value_ptr);

    assign_ret_val((*value_ptr) * (*value_ptr));
}

prjm_eel_function_decl(abs)
{
    assert_valid_ctx();

    ctx->value = .0;
    PRJM_EVAL_F* value_ptr = &ctx->value;

    invoke_arg(0, &value_ptr);

    assign_ret_val(fabs(*value_ptr));
}

prjm_eel_function_decl(min)
{
    assert_valid_ctx();

    PRJM_EVAL_F math_arg1 = .0;
    PRJM_EVAL_F math_arg2 = .0;
    PRJM_EVAL_F* math_arg1_ptr = &math_arg1;
    PRJM_EVAL_F* math_arg2_ptr = &math_arg2;

    invoke_arg(0, &math_arg1_ptr);
    invoke_arg(1, &math_arg2_ptr);

    assign_ret_val((*math_arg1_ptr) < (*math_arg2_ptr) ? (*math_arg1_ptr) : (*math_arg2_ptr));
}

prjm_eel_function_decl(max)
{
    assert_valid_ctx();

    PRJM_EVAL_F math_arg1 = .0;
    PRJM_EVAL_F math_arg2 = .0;
    PRJM_EVAL_F* math_arg1_ptr = &math_arg1;
    PRJM_EVAL_F* math_arg2_ptr = &math_arg2;

    invoke_arg(0, &math_arg1_ptr);
    invoke_arg(1, &math_arg2_ptr);

    assign_ret_val((*math_arg1_ptr) > (*math_arg2_ptr) ? (*math_arg1_ptr) : (*math_arg2_ptr));
}

prjm_eel_function_decl(sign)
{
    assert_valid_ctx();

    ctx->value = .0;
    PRJM_EVAL_F* value_ptr = &ctx->value;

    invoke_arg(0, &value_ptr);

    if (*value_ptr == 0)
    {
        (**ret_val) = .0;
        return;
    }
    assign_ret_val((*value_ptr) < .0 ? -1. : 1.);
}

prjm_eel_function_decl(rand)
{
    assert_valid_ctx();

    ctx->value = .0;
    PRJM_EVAL_F* value_ptr = &ctx->value;

    invoke_arg(0, &value_ptr);

    PRJM_EVAL_F rand_max = floor(*value_ptr);
    if (rand_max < 1.)
    {
        rand_max = 1.;
    }

    assign_ret_val((PRJM_EVAL_F) (prjm_eel_genrand_int32() * (1.0 / (double) 0xFFFFFFFF * rand_max)));
}

prjm_eel_function_decl(invsqrt)
{
    assert_valid_ctx();

    /*
     * Using fast inverse square root implementation here, same as Milkdrop, except supporting doubles.
     * See https://en.wikipedia.org/wiki/Fast_inverse_square_root
     */
#if PRJM_F_SIZE == 4
#define INVSQRT_MAGIC_NUMBER 0x5f3759df
#define INVSQRT_INT uint32_t
#else
#define INVSQRT_MAGIC_NUMBER 0x5fe6eb50c7b537a9
#define INVSQRT_INT uint64_t
#endif

    union
    {
        PRJM_EVAL_F PRJM_F_val;
        INVSQRT_INT int_val;
    } type_conv;

    static const PRJM_EVAL_F three_halfs = 1.5;
    static const PRJM_EVAL_F one_half = .5;


    ctx->value = .0;
    PRJM_EVAL_F* value_ptr = &ctx->value;

    invoke_arg(0, &value_ptr);

    PRJM_EVAL_F num2 = (*value_ptr) * one_half;
    type_conv.PRJM_F_val = (*value_ptr);
    type_conv.int_val = INVSQRT_MAGIC_NUMBER - (type_conv.int_val >> 1);
    type_conv.PRJM_F_val = type_conv.PRJM_F_val * (three_halfs - (num2 * type_conv.PRJM_F_val * type_conv.PRJM_F_val));

    assign_ret_val(type_conv.PRJM_F_val);
}
