%option reentrant
%option nounput noinput noyywrap 8bit nodefault yylineno
%option bison-bridge bison-locations
%option prefix="prjm_eel_"
%option extra-type="struct prjm_eel_scanner_state *"

%top{
#include "CompilerTypes.h"
#include "Compiler.h"

#include <stdio.h>

#define YY_STDINIT
}


    /* States */
%x ML_COMMENT
%x IL_COMMENT

    /* Rule definitions */
DIGIT           [0-9]
HEX             [0-9a-fA-F]
SIGN            [+-]
EXPO            [eE]{SIGN}?{DIGIT}+
FLOAT           ({DIGIT}+|{DIGIT}+\.{DIGIT}*|{DIGIT}*\.{DIGIT}+){EXPO}?
NAME            [_a-zA-Z][_a-zA-Z0-9]*

%%

    /* Comments */
"/*"                { printf("[Scanner] START COMMENT BLOCK\n"); BEGIN(ML_COMMENT); }
<ML_COMMENT>"*/"    { printf("[Scanner] END COMMENT BLOCK\n");BEGIN(INITIAL); }
<ML_COMMENT>\n      { }

"//"                { printf("[Scanner] START INLINE COMMENT\n");BEGIN(IL_COMMENT); }
<IL_COMMENT>\n      { printf("[Scanner] END INLINE COMMENT\n");BEGIN(INITIAL); }
<IL_COMMENT>.       { }

    /* Constants */
"$"[Xx]{HEX}+       {
                        printf("[Scanner] Found HEX Constant\n");
                        char* p = yytext + 2;
                        yylval->NUM = (double)strtoul(yytext + 2, &p, 16);
                        return NUM;
                        }
"$'"."'"            { printf("[Scanner] Found ORD Constant\n"); yylval->NUM = *(char*)(yytext + 2); return NUM; }
"$"(?i:pi)          { printf("[Scanner] Found PI Constant\n"); yylval->NUM = 3.141592653589793f; return NUM; }
"$"[eE]             { printf("[Scanner] Found E Constant\n"); yylval->NUM = 2.71828183f; return NUM; }
"$"(?i:phi)         { printf("[Scanner] Found PHI Constant\n"); yylval->NUM = 1.61803399f; return NUM; }

    /* Literals */
(?i:gmem)          { printf("[Scanner] Found GMEM\n"); return GMEM; }
(?i:gmegabuf)      { printf("[Scanner] Found GMEGABUF\n"); return GMEGABUF; }
(?i:megabuf)       { printf("[Scanner] Found MEGABUF\n"); return MEGABUF; }

{FLOAT}            {
                        yylval->NUM = atof(yytext);
                        printf("[Scanner] Found number: %g\n", yylval->NUM);
                        return NUM;
                    }
{NAME}              {
                        if (prjm_eel_compiler_name_is_function(cctx, yytext))
                        {
                            printf("[Scanner] Found FUNCTION: %s\n", yytext);
                            yylval->FUNC = strdup(yytext);
                            return FUNC;
                        }
                        printf("[Scanner] Found VARIABLE: %s\n", yytext);
                        yylval->VAR = strdup(yytext);
                        return VAR;
                    }

    /* Operators */
"+="                { printf("[Scanner] Found ADDOP\n"); return ADDOP; }
"-="                { printf("[Scanner] Found SUBOP\n"); return SUBOP; }
"%="                { printf("[Scanner] Found MODOP\n"); return MODOP; }
"|="                { printf("[Scanner] Found OROP\n"); return OROP; }
"&="                { printf("[Scanner] Found ANDOP\n"); return ANDOP; }
"/="                { printf("[Scanner] Found DIVOP\n"); return DIVOP; }
"*="                { printf("[Scanner] Found MULOP\n"); return MULOP; }
"^="                { printf("[Scanner] Found POWOP\n"); return POWOP; }
"=="                { printf("[Scanner] Found EQUAL\n"); return EQUAL; }
"<="                { printf("[Scanner] Found BELEQ\n"); return BELEQ; }
">="                { printf("[Scanner] Found ABOEQ\n"); return ABOEQ; }
"!="                { printf("[Scanner] Found NOTEQ\n"); return NOTEQ; }
"||"                { printf("[Scanner] Found BOOLOR\n"); return BOOLOR; }
"&&"                { printf("[Scanner] Found BOOLAND\n"); return BOOLAND; }
"<"	                { printf("[Scanner] Found BELOW\n"); return '<'; }
">"	                { printf("[Scanner] Found ABOVE\n"); return '>'; }
"+"	                { printf("[Scanner] Found PLUS\n"); return '+'; }
"-"	                { printf("[Scanner] Found MINUS\n"); return '-'; }
"*"	                { printf("[Scanner] Found MULTIPLY\n"); return '*'; }
"/"                 { printf("[Scanner] Found DIVIDE\n"); return '/'; }
"%"                 { printf("[Scanner] Found MODULO\n"); return '%'; }
"^"	                { printf("[Scanner] Found POW\n"); return '^'; }
"!"	                { printf("[Scanner] Found NOT\n"); return '!'; }
"="	                { printf("[Scanner] Found ASSIGN\n"); return '='; }

    /* Syntactic elements */
"("                 { printf("[Scanner] Found PAR_OPEN\n"); return '('; }
")"                 { printf("[Scanner] Found PAR_CLOSE\n"); return ')'; }
"["                 { printf("[Scanner] Found IDX_OPEN\n"); return '['; }
"]"                 { printf("[Scanner] Found IDX_CLOSE\n"); return ']'; }
"?"                 { printf("[Scanner] Found TERNARY_EXP\n"); return '?'; }
":"                 { printf("[Scanner] Found TERNARY_OR\n"); return ':'; }
","                 { printf("[Scanner] Found COMMA\n"); return ','; }

    /* Expression terminator */
";"			        { printf("[Scanner] Found SEMICOLON\n"); return ';'; }

    /* Ignored whitespace */
[ \r\n\t\v\f]	        { }

    /* All other tokens are invalid */
.                   { printf("[Scanner] Unknown Token!\n"); return PRJM_EEL_UNDEF; }

%%
