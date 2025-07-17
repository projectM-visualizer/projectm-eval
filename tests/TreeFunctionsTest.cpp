#include "TreeFunctionsTest.hpp"

#include <cmath>

#ifdef _MSC_VER
#define strcasecmp stricmp
#define strncasecmp _strnicmp
#endif

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif
#ifndef M_PI_2
    #define M_PI_2 1.5707963267948966
#endif

#if PRJM_F_SIZE == 4
#define ASSERT_PRJM_F_EQ ASSERT_FLOAT_EQ
#define EXPECT_PRJM_F_EQ EXPECT_FLOAT_EQ
#else
#define ASSERT_PRJM_F_EQ ASSERT_DOUBLE_EQ
#define EXPECT_PRJM_F_EQ EXPECT_DOUBLE_EQ
#endif

prjm_eval_variable_def_t* TreeFunctions::FindVariable(const char* name)
{
    for (const auto var: m_variables)
    {
        if (strcasecmp(name, var->name) == 0)
        {
            return var;
        }
    }

    return nullptr;
}

prjm_eval_variable_def_t* TreeFunctions::CreateVariable(const char* name, PRJM_EVAL_F initialValue)
{
    auto* var = FindVariable(name);
    if (!var)
    {
        var = new prjm_eval_variable_def_t;
    }

    var->name = strdup(name);
    var->value = initialValue;

    m_variables.push_back(var);

    return var;
}

prjm_eval_exptreenode_t* TreeFunctions::CreateEmptyNode(int argCount)
{
    auto* node = reinterpret_cast<prjm_eval_exptreenode_t*>(calloc(1, sizeof(prjm_eval_exptreenode_t)));
    if (argCount > 0)
    {
        node->args = reinterpret_cast<prjm_eval_exptreenode_t**>( calloc(argCount + 1,
                                                                         sizeof(prjm_eval_exptreenode_t*)));
    }
    return node;
}

prjm_eval_exptreenode_t* TreeFunctions::CreateConstantNode(PRJM_EVAL_F value)
{
    auto* varNode = CreateEmptyNode(0);

    varNode->func = prjm_eval_func_const;
    varNode->value = value;

    return varNode;
}

prjm_eval_exptreenode_t*
TreeFunctions::CreateVariableNode(const char* name, PRJM_EVAL_F initialValue, prjm_eval_variable_def_t** variable)
{
    *variable = CreateVariable(name, initialValue);

    auto* varNode = CreateEmptyNode(0);

    varNode->func = prjm_eval_func_var;
    varNode->var = &(*variable)->value;

    return varNode;
}

void TreeFunctions::SetUp()
{
    Test::SetUp();

}

void TreeFunctions::TearDown()
{
    for (auto node: m_treeNodes)
    {
        prjm_eval_destroy_exptreenode(node);
    }

    for (const auto var: m_variables)
    {
        free(var->name); // alloc'd via C malloc/strdup!
        delete var;
    }
    m_variables.clear();

    Test::TearDown();

}


TEST_F(TreeFunctions, Constant)
{
    auto* constNode = CreateConstantNode(5.0f);

    m_treeNodes.push_back(constNode);

    PRJM_EVAL_F value{};
    PRJM_EVAL_F* valuePointer = &value;
    constNode->func(constNode, &valuePointer);

    ASSERT_PRJM_F_EQ(*valuePointer, 5.);
}

TEST_F(TreeFunctions, Variable)
{
    prjm_eval_variable_def_t* var;
    auto* varNode = CreateVariableNode("x", 5.f, &var);

    m_treeNodes.push_back(varNode);

    PRJM_EVAL_F value{};
    PRJM_EVAL_F* valuePointer = &value;
    varNode->func(varNode, &valuePointer);

    EXPECT_PRJM_F_EQ(*valuePointer, 5.);
    ASSERT_EQ(valuePointer, &var->value);
}

TEST_F(TreeFunctions, DISABLED_ExecuteList)
{

    // Expression list ("x = -50; y = 50;")
    prjm_eval_variable_def_t* var1;
    auto* varNode1 = CreateVariableNode("x", 5.f, &var1);
    auto* constNode1 = CreateConstantNode(-50.0f);

    auto* setNode1 = CreateEmptyNode(2);
    setNode1->func = prjm_eval_func_set;
    setNode1->args[0] = varNode1;
    setNode1->args[1] = constNode1;


    prjm_eval_variable_def_t* var2;
    auto* varNode2 = CreateVariableNode("y", 123.f, &var2);
    auto* constNode2 = CreateConstantNode(50.0f);

    auto* setNode2 = CreateEmptyNode(2);
    setNode2->func = prjm_eval_func_set;
    setNode2->args[0] = varNode2;
    setNode2->args[1] = constNode2;

    // Executor
    auto* listNode = CreateEmptyNode(1);
    listNode->func = prjm_eval_func_execute_list;
    listNode->args[0] = setNode1;

    m_treeNodes.push_back(listNode);

    PRJM_EVAL_F value{};
    PRJM_EVAL_F* valuePointer = &value;
    listNode->func(listNode, &valuePointer);

    EXPECT_PRJM_F_EQ(*valuePointer, 50.);
    EXPECT_PRJM_F_EQ(var1->value, -50.);
    EXPECT_PRJM_F_EQ(var2->value, 50.);
}

TEST_F(TreeFunctions, Set)
{
    prjm_eval_variable_def_t* var1;
    prjm_eval_variable_def_t* var2;
    auto* varNode1 = CreateVariableNode("x", 5.f, &var1);
    auto* varNode2 = CreateVariableNode("y", 45.f, &var2);

    auto* setNode = CreateEmptyNode(2);
    setNode->func = prjm_eval_func_set;
    setNode->args[0] = varNode1;
    setNode->args[1] = varNode2;

    m_treeNodes.push_back(setNode);

    PRJM_EVAL_F value{};
    PRJM_EVAL_F* valuePointer = &value;
    setNode->func(setNode, &valuePointer);

    ASSERT_EQ(valuePointer, &var1->value);
    ASSERT_NE(valuePointer, &var2->value);
    EXPECT_PRJM_F_EQ(*valuePointer, 45.0);
    EXPECT_PRJM_F_EQ(var1->value, 45.0);
}


TEST_F(TreeFunctions, DivisionOperator)
{
    prjm_eval_variable_def_t* var1;
    prjm_eval_variable_def_t* var2;
    auto* varNode1 = CreateVariableNode("x", 5.f, &var1);
    auto* varNode2 = CreateVariableNode("y", 2.f, &var2);

    auto* divNode = CreateEmptyNode(2);
    divNode->func = prjm_eval_func_div;
    divNode->args[0] = varNode1;
    divNode->args[1] = varNode2;

    m_treeNodes.push_back(divNode);

    PRJM_EVAL_F value{};
    PRJM_EVAL_F* valuePointer = &value;
    divNode->func(divNode, &valuePointer);

    EXPECT_PRJM_F_EQ(*valuePointer, 2.5) << "Dividing 5.0 by 2.0";

    var1->value = 0.0;
    var2->value = 5.0;
    divNode->func(divNode, &valuePointer);
    EXPECT_PRJM_F_EQ(*valuePointer, 0.0) << "Dividing 0.0 by 5.0";

    // Division by 0 should return 0, not NaN
    var1->value = 5.0;
    var2->value = 0.0;
    divNode->func(divNode, &valuePointer);
    EXPECT_PRJM_F_EQ(*valuePointer, 0.0) << "Dividing 0.0 by 5.0 - expected 0.0, not NaN";
}

TEST_F(TreeFunctions, ASinFunction)
{
    prjm_eval_variable_def_t* var;
    auto* varNode = CreateVariableNode("x", 0.f, &var);

    auto* asinNode = CreateEmptyNode(1);
    asinNode->func = prjm_eval_func_asin;
    asinNode->args[0] = varNode;

    m_treeNodes.push_back(asinNode);

    PRJM_EVAL_F value{};
    PRJM_EVAL_F* valuePointer = &value;
    asinNode->func(asinNode, &valuePointer);

    EXPECT_PRJM_F_EQ(*valuePointer, 0.0) << "asin(0.0)";

    var->value = 1.0;
    asinNode->func(asinNode, &valuePointer);
    EXPECT_PRJM_F_EQ(*valuePointer,  M_PI_2) << "asin(1.0)";

    var->value = -1.0;
    asinNode->func(asinNode, &valuePointer);
    EXPECT_PRJM_F_EQ(*valuePointer, -M_PI_2) << "asin(-1.0)";

    var->value = 2.0;
    asinNode->func(asinNode, &valuePointer);
    EXPECT_PRJM_F_EQ(*valuePointer, 0.0) << "asin(2.0) - expected 0.0, not NaN";

    var->value = -2.0;
    asinNode->func(asinNode, &valuePointer);
    EXPECT_PRJM_F_EQ(*valuePointer, 0.0) << "asin(-2.0) - expected 0.0, not NaN";
}

TEST_F(TreeFunctions, ACosFunction)
{
    prjm_eval_variable_def_t* var;
    auto* varNode = CreateVariableNode("x", 0.f, &var);

    auto* acosNode = CreateEmptyNode(1);
    acosNode->func = prjm_eval_func_acos;
    acosNode->args[0] = varNode;

    m_treeNodes.push_back(acosNode);

    PRJM_EVAL_F value{};
    PRJM_EVAL_F* valuePointer = &value;
    acosNode->func(acosNode, &valuePointer);

    EXPECT_PRJM_F_EQ(*valuePointer, M_PI_2) << "acos(0.0)";

    var->value = 1.0;
    acosNode->func(acosNode, &valuePointer);
    EXPECT_PRJM_F_EQ(*valuePointer,  0.0) << "acos(1.0)";

    var->value = -1.0;
    acosNode->func(acosNode, &valuePointer);
    EXPECT_PRJM_F_EQ(*valuePointer, M_PI) << "acos(-1.0)";

    var->value = 2.0;
    acosNode->func(acosNode, &valuePointer);
    EXPECT_PRJM_F_EQ(*valuePointer, 0.0f) << "acos(2.0) - expected 0.0, not NaN";

    var->value = -2.0;
    acosNode->func(acosNode, &valuePointer);
    EXPECT_PRJM_F_EQ(*valuePointer, 0.0f) << "acos(-2.0) - expected 0.0, not NaN";
}

TEST_F(TreeFunctions, SqrFunction)
{
    prjm_eval_variable_def_t* var;
    auto* varNode = CreateVariableNode("x", 0.f, &var);

    auto* sqrNode = CreateEmptyNode(1);
    sqrNode->func = prjm_eval_func_sqr;
    sqrNode->args[0] = varNode;

    m_treeNodes.push_back(sqrNode);

    PRJM_EVAL_F value{};
    PRJM_EVAL_F* valuePointer = &value;
    sqrNode->func(sqrNode, &valuePointer);

    EXPECT_PRJM_F_EQ(*valuePointer, 0.0) << "sqr(0.0)";

    var->value = 1.0;
    sqrNode->func(sqrNode, &valuePointer);
    EXPECT_PRJM_F_EQ(*valuePointer, 1.0) << "sqr(1.0)";

    var->value = -1.0;
    sqrNode->func(sqrNode, &valuePointer);
    EXPECT_PRJM_F_EQ(*valuePointer, 1.0) << "sqr(-1.0)";

    var->value = 2.0;
    sqrNode->func(sqrNode, &valuePointer);
    EXPECT_PRJM_F_EQ(*valuePointer, 4.0) << "sqr(2.0)";

    var->value = -2.0;
    sqrNode->func(sqrNode, &valuePointer);
    EXPECT_PRJM_F_EQ(*valuePointer, 4.0) << "sqr(-2.0)";

    var->value = 1000000.0;
    sqrNode->func(sqrNode, &valuePointer);
    EXPECT_PRJM_F_EQ(*valuePointer, 1000000000000.0) << "sqr(1000000.0)";

    var->value = -1000000.0;
    sqrNode->func(sqrNode, &valuePointer);
    EXPECT_PRJM_F_EQ(*valuePointer, 1000000000000.0) << "sqr(-1000000.0)";

    var->value = 9999999999999.0;
    sqrNode->func(sqrNode, &valuePointer);
    EXPECT_PRJM_F_EQ(*valuePointer, 4611685743549480960.0) << "sqr(9999999999999.0)";

    // Overflow: should return Inf
#if PRJM_F_SIZE == 4
    // 10^4 below max value for a float
    var->value = 3.402823466E+34f;
#else
    // 10^8 below max value for a double
    var->value =  1.7976931348623157E+300;
#endif
    sqrNode->func(sqrNode, &valuePointer);
    EXPECT_PRJM_F_EQ(*valuePointer, 4611685743549480960.0) << "sqr(" << var->value << ")";

}

TEST_F(TreeFunctions, RandFunction)
{
    prjm_eval_variable_def_t* var;
    auto* varNode = CreateVariableNode("x", 0.f, &var);

    auto* randNode = CreateEmptyNode(1);
    randNode->func = prjm_eval_func_rand;
    randNode->args[0] = varNode;

    m_treeNodes.push_back(randNode);

    PRJM_EVAL_F value{};
    PRJM_EVAL_F* valuePointer = &value;

    for (int i = 0; i < 100; i++)
    {
        randNode->func(randNode, &valuePointer);
        EXPECT_LT(*valuePointer, 1.0) << "rand(0.0)";
    }

    auto testValue = [&var, &randNode, &valuePointer](PRJM_EVAL_F value)
    {
        var->value = value;
        bool oneValueOverThreshold = false;
        PRJM_EVAL_F threshold = value / 2.0;
        for (int i = 0; i < 100; i++)
        {
            randNode->func(randNode, &valuePointer);
            EXPECT_LT(*valuePointer, value) << "rand(" << value << ")";
            if (*valuePointer > threshold)
            {
                oneValueOverThreshold = true;
            }
        }
        EXPECT_TRUE(oneValueOverThreshold) << "One value must be over " << threshold;
    };

    testValue(100.0);
    testValue(10000.0);
    testValue(1000000.0);
    testValue(100000000.0);
    testValue(INT32_MAX);
    testValue(INT32_MAX * 1000000000000000.0);
}