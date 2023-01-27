%{
#include "Calc.h"
#include "Compiler.h"

#include <stdio.h>

%}

%option noyywrap
    /*%option yylineno*/
%option bison-bridge bison-locations
%option prefix="prjm_eel_"

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
"/*"                { BEGIN(ML_COMMENT); }
<ML_COMMENT>"*/"    { BEGIN(INITIAL); }
<ML_COMMENT>\n      { }

"//"                { BEGIN(IL_COMMENT); }
<IL_COMMENT>\n      { BEGIN(INITIAL); }
<IL_COMMENT>.       { }

    /* Constants */
"$"[Xx]{HEX}{HEX}*  {
                        printf("Found HEX Constant\n");
                        char* p = yytext + 2;
                        yylval->NUM = (double)strtoul(yytext + 2, &p, 16);
                        return NUM;
                        }
"$'"."'"            { printf("Found ORD Constant\n"); yylval->NUM = 0.0f; return NUM; }
"$"(?i:pi)          { printf("Found PI Constant\n"); yylval->NUM = 3.141592653589793f; return NUM; }
"$"[eE]             { printf("Found E Constant\n"); yylval->NUM = 2.71828183f; return NUM; }
"$"(?i:phi)         { printf("Found PHI Constant\n"); yylval->NUM = 1.61803399f; return NUM; }

    /* Literals */
(?i:loop)          { printf("Found LOOP\n"); return LOOP; }
(?i:while)         { printf("Found WHILE\n"); return WHILE; }
(?i:gmem)          { printf("Found GMEM\n"); return GMEM; }
(?i:gmegabuf)      { printf("Found GMEGABUF\n"); return GMEGABUF; }
(?i:megabuf)       { printf("Found MEGABUF\n"); return MEGABUF; }
{FLOAT}            {
                        yylval->NUM = atof(yytext);
                        printf("Found number: %g\n", yylval->NUM);
                        return NUM;
                    }
{NAME}              {
                        printf("Found text: %s\n", yytext);
                        symrec *s = getsym(yytext);
                        if (!s)
                            s = putsym(yytext, VAR);
                        yylval->VAR = s;
                        return s->type;
                    }

    /* Operators */
"+="                { printf("Found ADDOP\n"); return ADDOP; }
"-="                { printf("Found SUBOP\n"); return SUBOP; }
"%="                { printf("Found MODOP\n"); return MODOP; }
"|="                { printf("Found OROP\n"); return OROP; }
"&="                { printf("Found ANDOP\n"); return ANDOP; }
"/="                { printf("Found DIVOP\n"); return DIVOP; }
"*="                { printf("Found MULOP\n"); return MULOP; }
"^="                { printf("Found POWOP\n"); return POWOP; }
"=="                { printf("Found EQUAL\n"); return EQUAL; }
"<="                { printf("Found BELEQ\n"); return BELEQ; }
">="                { printf("Found ABOEQ\n"); return ABOEQ; }
"!="                { printf("Found NOTEQ\n"); return NOTEQ; }
"||"                { printf("Found BOOLOR\n"); return BOOLOR; }
"&&"                { printf("Found BOOLAND\n"); return BOOLAND; }
"<"	                { printf("Found BELOW\n"); return '<'; }
">"	                { printf("Found ABOVE\n"); return '>'; }
"+"	                { printf("Found PLUS\n"); return '+'; }
"-"	                { printf("Found MINUS\n"); return '-'; }
"*"	                { printf("Found MULTIPLY\n"); return '*'; }
"/"                 { printf("Found DIVIDE\n"); return '/'; }
"%"                 { printf("Found MODULO\n"); return '%'; }
"^"	                { printf("Found POW\n"); return '^'; }
"!"	                { printf("Found NOT\n"); return '!'; }
"="	                { printf("Found ASSIGN\n"); return '='; }

    /* Syntactic elements */
"("                 { printf("Found PAR_OPEN\n"); return '('; }
")"                 { printf("Found PAR_CLOSE\n"); return ')'; }
"["                 { printf("Found IDX_OPEN\n"); return '['; }
"]"                 { printf("Found IDX_CLOSE\n"); return ']'; }
"?"                 { printf("Found TERNARY_EXP\n"); return '?'; }
":"                 { printf("Found TERNARY_OR\n"); return ':'; }
","                 { printf("Found COMMA\n"); return ','; }

    /* Expression terminator */
";"			        { printf("Found SEMICOLON\n"); return ';'; }

    /* Ignored whitespace */
[ \r\n\t\v\f]	        { }

    /* All other tokens are invalid */
.                   { printf("Unknown Token!\n"); return PRJM_EEL_UNDEF; }

%%
