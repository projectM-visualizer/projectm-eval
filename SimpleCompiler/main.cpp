extern "C" {
#include "projectm-eval/CompileContext.h"
#include "projectm-eval/CompilerTypes.h"
#include "projectm-eval/MemoryBuffer.h"
}

#include "FileParser.hpp"

#include <iostream>

// Empty mutex functions
void prjm_eel_memory_host_lock_mutex()
{
}

void prjm_eel_memory_host_unlock_mutex()
{
}

static void DumpVars(prjm_eel_compiler_context_t* cctx)
{
    printf("\nDumping Vars:\n");
    prjm_eel_variable_entry_t* item = cctx->variables.first;
    while (item)
    {
        printf("%s => %g\n", item->variable->name, item->variable->value);
        item = item->next;
    }
}

static void ScanString(const char* input)
{
    prjm_eel_compiler_context_t* cctx = prjm_eel_create_compile_context(nullptr);
    prjm_eel_program_t* program = prjm_eel_compile_code(cctx, input);

    if (!program)
    {
        int line, column;
        auto* error = prjm_eel_compiler_get_error(cctx, &line, &column);
        std::cout << "Parsing failed: " << error << " (Line " << line << ", Column " << column << ")" << std::endl;
    }
    else
    {
        std::cout << "Parsing done, running." << std::endl;

        PRJM_EEL_F init_value = .0;
        PRJM_EEL_F* init_value_ptr = &init_value;
        program->program->func(program->program, &init_value_ptr);

        std::cout << "Program executed, returned " << *init_value_ptr << "." << std::endl;

        DumpVars(cctx);

        prjm_eel_destroy_code(program);
    }

    prjm_eel_destroy_compile_context(cctx);
}

int main(int argc, char const* argv[])
{
    if (argc < 3)
    {
        std::cerr << "Usage: " << argv[0] << " [preset file] [prefix]" << std::endl;
        return 1;
    }

    FileParser parser;
    if (!parser.Read(argv[1]))
    {
        std::cerr << "Read error." << std::endl;
        return 1;
    }
    std::cout << "Code:" << std::endl << parser.GetCode(argv[2]) << std::endl << std::endl;
    ScanString(parser.GetCode(argv[2]).c_str());

    prjm_eel_memory_destroy_global();

    return 0;
}
