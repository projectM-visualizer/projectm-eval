#pragma once

extern "C"
{
#include "projectm-eel/TreeFunctions.h"
};

#include <gtest/gtest.h>

#include <vector>

class TreeFunctions : public testing::Test
{
public:

protected:
    prjm_eel_variable_def_t* FindVariable(const char* name);

    prjm_eel_variable_def_t* CreateVariable(const char* name, PRJM_EEL_F initialValue);

    prjm_eel_exptreenode_t* CreateEmptyNode(int argCount);

    prjm_eel_exptreenode_t* CreateConstantNode(PRJM_EEL_F value);

    prjm_eel_exptreenode_t* CreateVariableNode(const char* name,
                                               PRJM_EEL_F initialValue,
                                               prjm_eel_variable_def_t** variable);

    void SetUp() override;

    void TearDown() override;

    std::vector<prjm_eel_variable_def_t*> m_variables{};
    std::vector<prjm_eel_exptreenode_t*> m_treeNodes;
};