extern "C" {
#include "CompileContext.h"
#include "CompilerTypes.h"
#include "Compiler.h"
#include "Scanner.h"
}

#include "FileParser.hpp"

#include <iostream>

static void DumpVars(prjm_eel_compiler_context_t* cctx)
{
    printf("\nDumping Vars:\n");
    prjm_eel_variable_entry_t* item = cctx->variables.first;
    while (item)
    {
        printf("%s => %f\n", item->variable->name, *item->variable->value_ptr);
        item = item->next;
    }
}

static int ScanString(const char* input)
{
    prjm_eel_compiler_context_t* cctx = prjm_eel_create_compile_context();
    yyscan_t scanner{};

    prjm_eel_lex_init(&scanner);
    YY_BUFFER_STATE bufferState = prjm_eel__scan_string(input, scanner);

    int result = prjm_eel_parse(cctx, scanner);

    prjm_eel__delete_buffer(bufferState, scanner);
    prjm_eel_lex_destroy(scanner);

    std::cout << "Parsing done, running." << std::endl;

    if (cctx->program)
    {
        float init_value = .0f;
        float* init_value_ptr = &init_value;
        cctx->program->func(cctx->program, &init_value_ptr);

        std::cout << "Program executed, returned " << *init_value_ptr << "." << std::endl;

        DumpVars(cctx);
    }

    prjm_eel_destroy_compile_context(cctx);

    return result;
}

int main(int argc, char const* argv[])
{
    if (argc < 3)
    {
        std::cerr << "Usage: " << argv[0] << " [preset file] [prefix]" << std::endl;
        return 1;
    }

    int result{};
    {
        {
            FileParser parser;
            if (!parser.Read(argv[1]))
            {
                std::cerr << "Read error." << std::endl;
                return 1;
            }
            result = ScanString(parser.GetCode(argv[2]).c_str());
        }

    }

    return result;
}
