#pragma once

#ifdef __cplusplus
extern "C" {
#endif

struct prjm_eel_exptreenode;

/**
 * @brief Node function for a single expression.
 */
typedef void (prjm_eel_expr_func_t)(struct prjm_eel_exptreenode* ctx, float** ret_val);

/**
 * @brief Math function pointer with one argument.
 */
typedef float (prjm_eel_math_func1)(float);

/**
 * @brief Math function pointer with two arguments.
 */
typedef float (prjm_eel_math_func2)(float, float);


/**
 * @brief A user-defined or external variable.
 * The value_ptr member is used to reference the actual contents. It will point to int_value
 * if the variable is script-defined, or to any external value if registered from the outside.
 * Variables are stored as a linked list for efficiency.
 */
typedef struct prjm_eel_variable
{
    char* name;
    float* value_ptr;
    float value;
    struct prjm_eel_variable* next;
} prjm_eel_variable_t;

/**
 * @brief A single function, variable or constant in the expression tree.
 * The assigned function will determine how to access the other members.
 */
typedef struct prjm_eel_exptreenode
{
    prjm_eel_expr_func_t* func;
    void* math_func;
    float value; /*!< A constant, numerical value. Also used as temp value. */
    union
    {
        prjm_eel_variable_t* var; /*!< Variable reference. */
        struct prjm_eel_exptreenode* arg1; /*!< Sub-expression for first function argument. */
    };
    struct prjm_eel_exptreenode* arg2; /*!< Sub-expression for second function argument. */
    struct prjm_eel_exptreenode* arg3; /*!< Sub-expression for third function argument. */
    struct prjm_eel_exptreenode* next; /*!< Linked list of sub-expressions for instruction list execution. */
} prjm_eel_exptreenode_t;

/**
 * @brief Execution context.
 * Holds the root node of the expression tree and a pointer to the variable list.
 */
typedef struct prjm_eel_context
{
    prjm_eel_exptreenode_t* program;
    prjm_eel_variable_t* variables;
} prjm_eel_context_t;

#ifdef __cplusplus
};
#endif
