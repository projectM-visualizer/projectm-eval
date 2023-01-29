#pragma once

#include "TreeFunctions.h"

#include <gtest/gtest.h>

#include <vector>

class TreeFunctions : public testing::Test
{
public:

protected:
    prjm_eel_variable_t* FindVariable(const char* name);

    prjm_eel_variable_t* CreateVariable(const char* name, float initialValue, float* externalDataPointer);

    prjm_eel_exptreenode_t* CreateEmptyNode();

    prjm_eel_exptreenode_t* CreateConstantNode(float value);

    prjm_eel_exptreenode_t* CreateVariableNode(const char* name, float initialValue, float* externalDataPointer,
                                               prjm_eel_variable_t** variable);

    void SetUp() override;

    void TearDown() override;

    prjm_eel_variable_t* m_variables{};
    std::vector<prjm_eel_exptreenode_t*> n_treeNodes;
};