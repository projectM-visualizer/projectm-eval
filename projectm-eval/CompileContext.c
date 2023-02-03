#include "CompileContext.h"

#include "Scanner.h"
#include "Compiler.h"
#include "MemoryBuffer.h"
#include "TreeFunctions.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

prjm_eel_compiler_context_t* prjm_eel_create_compile_context(prjm_eel_mem_buffer global_memory)
{
    prjm_eel_compiler_context_t* cctx = calloc(1, sizeof(prjm_eel_compiler_context_t));

    prjm_eel_intrinsic_function_list intrinsics;
    int intrinsics_count = 0;
    prjm_eel_intrinsic_functions(&intrinsics, &intrinsics_count);

    assert(intrinsics);
    assert(intrinsics_count);

    prjm_eel_function_list_item_t* last_func = NULL;
    for(int index = intrinsics_count - 1; index >= 0; --index)
    {
        assert(&intrinsics[index]);
        prjm_eel_function_list_item_t* func = malloc(sizeof(prjm_eel_function_list_item_t));
        func->function = malloc(sizeof(prjm_eel_function_def_t));
        memcpy(func->function, &intrinsics[index], sizeof(prjm_eel_function_def_t));
        func->function->name = strdup(intrinsics[index].name);
        func->next = last_func;
        last_func = func;
    }
    cctx->functions.first = last_func;

    cctx->memory = prjm_eel_memory_create_buffer();

    if (global_memory)
    {
        cctx->global_memory = global_memory;
    }
    else
    {
        cctx->global_memory = prjm_eel_memory_global();
    }

    return cctx;
}

void prjm_eel_destroy_compile_context(prjm_eel_compiler_context_t* cctx)
{
    assert(cctx);

    prjm_eel_function_list_item_t* func = cctx->functions.first;
    while(func)
    {
        prjm_eel_function_list_item_t* free_func = func;
        func = func->next;

        free(free_func->function->name);
        free(free_func->function);
        free(free_func);
    }

    prjm_eel_variable_entry_t* var = cctx->variables.first;
    while(var)
    {
        prjm_eel_variable_entry_t* free_var = var;
        var = var->next;

        free(free_var->variable->name);
        free(free_var->variable);
        free(free_var);
    }

    prjm_eel_destroy_exptreenode(cctx->compile_result);
    prjm_eel_memory_destroy_buffer(cctx->memory);

    free(cctx->error.error);

    free(cctx);
}

prjm_eel_program_t* prjm_eel_compile_code(prjm_eel_compiler_context_t* cctx, const char* code)
{
    yyscan_t scanner;

    prjm_eel_lex_init(&scanner);
    YY_BUFFER_STATE bufferState = prjm_eel__scan_string(code, scanner);

    bufferState->yy_bs_lineno = 1;
    bufferState->yy_bs_column = 0;

    int result = prjm_eel_parse(cctx, scanner);

    prjm_eel__delete_buffer(bufferState, scanner);
    prjm_eel_lex_destroy(scanner);

    if (result > 0)
    {
        prjm_eel_destroy_exptreenode(cctx->compile_result);
        cctx->compile_result = NULL;
        return NULL;
    }

    prjm_eel_program_t* program = malloc(sizeof(prjm_eel_program_t));
    program->cctx = cctx;
    program->program = cctx->compile_result;
    cctx->compile_result = NULL;

    return program;
}

void prjm_eel_destroy_code(prjm_eel_program_t* program)
{
    if (!program)
    {
        return;
    }

    prjm_eel_destroy_exptreenode(program->program);
    free(program);
}

void prjm_eel_reset_context_vars(prjm_eel_compiler_context_t* cctx)
{
    assert(cctx);

    prjm_eel_variable_entry_t* var = cctx->variables.first;
    while (var)
    {
        var->variable->value = .0f;
        var = var->next;
    }
}

const char* prjm_eel_compiler_get_error(prjm_eel_compiler_context_t* cctx, int* line, int* column)
{
    assert(cctx);

    if (line)
    {
        *line = cctx->error.line;
    }

    if (column)
    {
        *column = cctx->error.column;
    }

    return cctx->error.error;
}