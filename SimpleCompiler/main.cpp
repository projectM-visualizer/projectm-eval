extern "C" {
#include "CompilerTypes.h"
#include "Compiler.h"
#include "Scanner.h"
}

#include <iostream>
#include <sstream>

static int ScanString(const char* input)
{
    int result{};
    prjm_eel_compiler_context_t cctx;
    yyscan_t scanner{};

    prjm_eel_lex_init(&scanner);
    YY_BUFFER_STATE bufferState = prjm_eel__scan_string(input, scanner);

    result = prjm_eel_parse(&cctx, scanner);

    prjm_eel__delete_buffer(bufferState, scanner);
    prjm_eel_lex_destroy(scanner);

    return result;
}

int main (int argc, char const* argv[])
{
    int result{};
    {
        {
            std::stringstream programStream;
            programStream << std::cin.rdbuf();
            result = ScanString(programStream.str().c_str());
        }

        std::cout << "Parsing done." << std::endl;
    }

    return result;
}
