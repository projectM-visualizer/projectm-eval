extern "C" {
#include "CompilerTypes.h"
#include "Compiler.h"
#include "Scanner.h"
}

int main (int argc, char const* argv[])
{
    prjm_eel_compiler_context_t cctx;
    yyscan_t scanner;
    prjm_eel_lex_init(&scanner);

    return prjm_eel_parse(&cctx, &scanner);
}