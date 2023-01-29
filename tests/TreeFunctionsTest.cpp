#include "TreeFunctionsTest.hpp"

#include <cmath>

prjm_eel_variable_t* TreeFunctions::FindVariable(const char* name)
{
    prjm_eel_variable_t* var = m_variables;

    while (var)
    {
        if (strcmp(var->name, name) == 0)
        {
            return var;
        }
        var = var->next;
    }

    return var;
}

prjm_eel_variable_t* TreeFunctions::CreateVariable(const char* name, float initialValue, float* externalDataPointer)
{
    auto* var = FindVariable(name);
    if (!var)
    {
        var = new prjm_eel_variable;
    }

    var->name = strdup(name);
    var->value = initialValue;
    var->value_ptr = externalDataPointer ? externalDataPointer : &var->value;
    var->next = m_variables;

    m_variables = var;

    return var;
}

prjm_eel_exptreenode_t* TreeFunctions::CreateEmptyNode()
{
    auto* varNode = new prjm_eel_exptreenode{};

    n_treeNodes.push_back(varNode);

    return varNode;
}

prjm_eel_exptreenode_t* TreeFunctions::CreateConstantNode(float value)
{
    auto* varNode = CreateEmptyNode();

    varNode->func = prjm_eel_func_const;
    varNode->value = value;

    return varNode;
}

prjm_eel_exptreenode_t*
TreeFunctions::CreateVariableNode(const char* name, float initialValue, float* externalDataPointer,
                                  prjm_eel_variable_t** variable)
{
    *variable = CreateVariable(name, initialValue, externalDataPointer);

    auto* varNode = CreateEmptyNode();

    varNode->func = prjm_eel_func_var;
    varNode->var = *variable;

    return varNode;
}

void TreeFunctions::SetUp()
{
    Test::SetUp();

}

void TreeFunctions::TearDown()
{
    prjm_eel_variable_t* nextVar = m_variables;

    for (auto node: n_treeNodes)
    {
        delete node;
    }

    while (nextVar)
    {
        auto curVar = nextVar;
        nextVar = nextVar->next;
        free(curVar->name); // alloc'd via C malloc/strdup!
        delete curVar;
    }
    m_variables = nullptr;

    Test::TearDown();

}


TEST_F(TreeFunctions, Constant)
{
    auto* constNode = CreateConstantNode(5.0f);

    float value{};
    float* valuePointer = &value;
    constNode->func(constNode, &valuePointer);

    ASSERT_EQ(*valuePointer, 5.f);
}

TEST_F(TreeFunctions, Variable)
{
    prjm_eel_variable_t* var;
    auto* varNode = CreateVariableNode("x", 5.f, nullptr, &var);

    float value{};
    float* valuePointer = &value;
    varNode->func(varNode, &valuePointer);

    ASSERT_EQ(*valuePointer, 5.f);
    ASSERT_EQ(valuePointer, var->value_ptr);
}

TEST_F(TreeFunctions, VariableExternal)
{
    float externalValue = 10.f;

    prjm_eel_variable_t* var;
    auto* varNode = CreateVariableNode("x", 5.f, nullptr, &var);
    var->value_ptr = &externalValue;

    float value{};
    float* valuePointer = &value;
    varNode->func(varNode, &valuePointer);

    ASSERT_EQ(*valuePointer, 10.f);
    ASSERT_EQ(valuePointer, &externalValue);
}

TEST_F(TreeFunctions, ExecuteList)
{
    auto* constNode1 = CreateConstantNode(-50.0f);
    auto* constNode2 = CreateConstantNode(50.0f);

    prjm_eel_variable_t* var1;
    auto* varNode1 = CreateVariableNode("x", 5.f, nullptr, &var1);

    prjm_eel_variable_t* var2;
    auto* varNode2 = CreateVariableNode("y", 123.f, nullptr, &var2);

    auto* setNode1 = CreateEmptyNode();
    setNode1->func = prjm_eel_func_set;
    setNode1->arg1 = varNode1;
    setNode1->arg2 = constNode1;

    auto* setNode2 = CreateEmptyNode();
    setNode2->func = prjm_eel_func_set;
    setNode2->arg1 = varNode2;
    setNode2->arg2 = constNode2;

    auto* listNode = CreateEmptyNode();
    listNode->func = prjm_eel_func_execute_list;
    listNode->next = setNode1;
    setNode1->next = setNode2;

    float value{};
    float* valuePointer = &value;
    listNode->func(listNode, &valuePointer);

    ASSERT_EQ(*valuePointer, 50.f);
    ASSERT_FLOAT_EQ(var1->value, -50.f);
    ASSERT_FLOAT_EQ(var2->value, 50.f);
}

TEST_F(TreeFunctions, MathFunctionsOneArgument)
{
    auto* constNode = CreateConstantNode(5.0f);
    auto* sinNode = CreateEmptyNode();

    sinNode->func = prjm_eel_func_math_func1;
    sinNode->math_func = (void*)sinf;
    sinNode->arg1 = constNode;

    float value{};
    float* valuePointer = &value;
    sinNode->func(sinNode, &valuePointer);

    ASSERT_FLOAT_EQ(*valuePointer, -0.958924274663f);
}

TEST_F(TreeFunctions, MathFunctionsTwoArguments)
{
    auto* constNode1 = CreateConstantNode(5.0f);
    auto* constNode2 = CreateConstantNode(-5.0f);
    auto* atan2Node = CreateEmptyNode();

    atan2Node->func = prjm_eel_func_math_func2;
    atan2Node->math_func = (void*)atan2f;
    atan2Node->arg1 = constNode1;
    atan2Node->arg2 = constNode2;

    float value{};
    float* valuePointer = &value;
    atan2Node->func(atan2Node, &valuePointer);

    ASSERT_FLOAT_EQ(*valuePointer, 2.356194490192f);
    ASSERT_FLOAT_EQ(value, 2.356194490192f);
}

TEST_F(TreeFunctions, Set)
{
    prjm_eel_variable_t* var1;
    prjm_eel_variable_t* var2;
    auto* varNode1 = CreateVariableNode("x", 5.f, nullptr, &var1);
    auto* varNode2 = CreateVariableNode("y", 45.f, nullptr, &var2);
    auto* setNode = CreateEmptyNode();

    setNode->func = prjm_eel_func_set;
    setNode->arg1 = varNode1;
    setNode->arg2 = varNode2;

    float value{};
    float* valuePointer = &value;
    setNode->func(setNode, &valuePointer);

    ASSERT_EQ(valuePointer, var1->value_ptr);
    ASSERT_NE(valuePointer, var2->value_ptr);
    ASSERT_FLOAT_EQ(*valuePointer, 45.0f);
    ASSERT_FLOAT_EQ(var1->value, 45.0f);
}
