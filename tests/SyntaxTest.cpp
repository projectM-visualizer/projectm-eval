#include "SyntaxTest.hpp"


void SyntaxTest::SetUp()
{
    m_globalMemory = projectm_eval_memory_buffer_create();
    m_context = projectm_eval_context_create(m_globalMemory, &m_globalRegisters);
}

void SyntaxTest::TearDown()
{
    projectm_eval_context_destroy(m_context);
    projectm_eval_memory_buffer_destroy(m_globalMemory);
    memset(&m_globalRegisters, 0, sizeof(m_globalRegisters));
}

TEST_F(SyntaxTest, AdditionalLineBreak)
{
    const auto code = projectm_eval_code_compile(m_context, "k1 =  is_\nbeat*equal(index%2,0);");
    ASSERT_EQ(code, nullptr);
    ASSERT_STREQ(projectm_eval_get_error(m_context, nullptr, nullptr), "syntax error, unexpected VAR");
}

TEST_F(SyntaxTest, ParseFloatOneDot)
{
    const auto code = projectm_eval_code_compile(m_context, "1.0");
    ASSERT_FLOAT_EQ(projectm_eval_code_execute(code), 1.0f);
    projectm_eval_code_destroy(code);
}

TEST_F(SyntaxTest, ParseFloatOneDotZero)
{
    const auto code = projectm_eval_code_compile(m_context, "2.0");
    ASSERT_NE(code, nullptr);
    ASSERT_FLOAT_EQ(projectm_eval_code_execute(code), 2.0f);
    projectm_eval_code_destroy(code);
}

TEST_F(SyntaxTest, ParseFloatDotOne)
{
    const auto code = projectm_eval_code_compile(m_context, ".1");
    ASSERT_NE(code, nullptr);
    ASSERT_FLOAT_EQ(projectm_eval_code_execute(code), 0.1f);
    projectm_eval_code_destroy(code);
}

TEST_F(SyntaxTest, ParseFloatZeroDotOne)
{
    const auto code = projectm_eval_code_compile(m_context, "0.2");
    ASSERT_NE(code, nullptr);
    ASSERT_FLOAT_EQ(projectm_eval_code_execute(code), 0.2f);
    projectm_eval_code_destroy(code);
}

TEST_F(SyntaxTest, ParseFloatExponentWithNoDecimalInBase)
{
    auto code = projectm_eval_code_compile(m_context, "1e1");
    ASSERT_NE(code, nullptr);
    ASSERT_FLOAT_EQ(projectm_eval_code_execute(code), 10.0f);
    projectm_eval_code_destroy(code);

    code = projectm_eval_code_compile(m_context, "1E2");
    ASSERT_NE(code, nullptr);
    ASSERT_FLOAT_EQ(projectm_eval_code_execute(code), 100.0f);
    projectm_eval_code_destroy(code);
}

TEST_F(SyntaxTest, ParseFloatExponentWithDecimalInBase)
{
    auto code = projectm_eval_code_compile(m_context, "2.0e1");
    ASSERT_NE(code, nullptr);
    ASSERT_FLOAT_EQ(projectm_eval_code_execute(code), 20.0f);
    projectm_eval_code_destroy(code);

    code = projectm_eval_code_compile(m_context, "2.0E2");
    ASSERT_NE(code, nullptr);
    ASSERT_FLOAT_EQ(projectm_eval_code_execute(code), 200.0f);
    projectm_eval_code_destroy(code);
}

TEST_F(SyntaxTest, ParseInt)
{
    const auto code = projectm_eval_code_compile(m_context, "100");
    ASSERT_NE(code, nullptr);
    ASSERT_FLOAT_EQ(projectm_eval_code_execute(code), 100.0f);
    projectm_eval_code_destroy(code);
}

TEST_F(SyntaxTest, ParseIntOverflow)
{
    const auto code = projectm_eval_code_compile(m_context, "2147483648");
    ASSERT_NE(code, nullptr);
    ASSERT_FLOAT_EQ(projectm_eval_code_execute(code), INT32_MIN);
    projectm_eval_code_destroy(code);
}

// Only makes sense if the floating-point value has a larger range, e.g. double is used.
#if PRJM_F_SIZE == 8
TEST_F(SyntaxTest, ParseFloatNoOverflow)
{
    const auto code = projectm_eval_code_compile(m_context, "2147483648.0");
    ASSERT_NE(code, nullptr);
    ASSERT_FLOAT_EQ(projectm_eval_code_execute(code), 2147483648.0f);
    projectm_eval_code_destroy(code);
}
#endif
