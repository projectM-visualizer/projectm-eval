%code requires {

#include "CompilerFunctions.h"

#include <stdio.h>

typedef void* yyscan_t;
}

/* Generator options */
%verbose
%locations
%no-lines
%define api.pure true
%define api.prefix {prjm_eel_}
%define api.value.type union /* Generate YYSTYPE from these types: */
%define parse.error verbose

    /* Parser and lexer arguments */
%param {prjm_eel_compiler_context_t* cctx} { yyscan_t scanner }

%code provides {

    #define YYSTYPE PRJM_EEL_STYPE
    #define YYLTYPE PRJM_EEL_LTYPE

   #define YY_DECL \
       int yylex(YYSTYPE* yylval_param, YYLTYPE* yylloc_param, prjm_eel_compiler_context_t* cctx, yyscan_t yyscanner)
   YY_DECL;

   int yyerror(YYLTYPE* yyllocp, prjm_eel_compiler_context_t* cctx, yyscan_t yyscanner, const char* message);
}

/* Token declarations */

/* Special functions */
%token LOOP WHILE GMEM GMEGABUF MEGABUF

/* Operator tokens */
%token ADDOP SUBOP MODOP OROP ANDOP DIVOP MULOP POWOP EQUAL BELEQ ABOEQ NOTEQ BOOLOR BOOLAND

/* Value types */
%token <float> NUM
%token <char*> VAR FUNC
%nterm <prjm_eel_compiler_node_t*> variable function memory lvalue program instruction-list expression parentheses
%nterm <prjm_eel_compiler_arg_list_t*> function-arglist

/* Operator precedence, lowest first, highest last. */
%precedence ','
%right '='
%right ADDOP SUBOP
%right MULOP DIVOP MODOP
%right POWOP OROP ANDOP
%right '?' ':'
%left BOOLOR
%left BOOLAND
%left '|'
/* XOR would go here, but Milkdrop defines ^ as pow() and provides no XOR operator or function. */
%left '&'
%left EQUAL NOTEQ
%left '>' ABOEQ
%left '<' BELEQ
%left '-' '+'
%left '*' '/' '%'
%right '!'
%precedence NEG POS /* unary minus or plus */
%left '^'
%left '[' ']'
%left '(' ')'

%% /* The grammar follows. */

program:
  instruction-list[topnode] { printf ("[Parser] PROGRAM END\n"); if($topnode) { cctx->program = $topnode->tree_node; } }
;

/* Functions */
function:
  FUNC '(' function-arglist ')'            { printf("[Parser] FUNCTION: %s\n", $1); PRJM_EEL_FUNC($$, $1, $3); free($1); }
;

function-arglist:
  instruction-list                         { printf("[Parser] FUNCTION ARGLIST START\n"); $$ = prjm_eel_compiler_add_argument(NULL, $1); }
| function-arglist ',' instruction-list    { printf("[Parser] FUNCTION ARGLIST +ARG\n"); $$ = prjm_eel_compiler_add_argument($1, $3); }
;

parentheses:
  '(' instruction-list ')'                 { printf("[Parser] PARENTHESED EXPRESSION\n"); $$ = $2; }
| '(' ')'                                  { printf("[Parser] EMPTY PARENTHESES\n"); }
;

instruction-list:
  expression                               { printf ("[Parser] INSTRUCTION LIST START\n"); $$ = $1; }
| instruction-list[list] ';' expression[expr]    { printf ("[Parser] INSTRUCTION LIST +1\n"); $$ = prjm_eel_compiler_add_instruction(cctx, $list, $expr); }
| instruction-list[list] ';' empty-expression    { printf ("[Parser] INSTRUCTION LIST +0\n"); $$ = $1; }
| instruction-list[list] ';' error               { printf ("[Parser] INSTRUCTION LIST ERROR +0\n"); $$ = $1; }
;

/* Memory access */
memory:
/* Memory access via index */
  expression[idx] '[' ']'                    { printf("[Parser] INDEX[] - MEM AT INDEX <EXP>\n"); PRJM_EEL_FUNC1($$, "_mem", $idx) }
| expression[idx] '[' expression[offs] ']'   { printf("[Parser] INDEX[OFFSET] - MEM AT INDEX <EXP> WITH OFFSET <EXP>\n"); PRJM_EEL_FUNC2($$, "_addop", $idx, $offs); PRJM_EEL_FUNC1($$, "_mem", $$) }
| GMEM '[' expression[idx] ']'               { printf("[Parser] GMEM[INDEX] - GLOBAL MEM AT INDEX <EXP>\n"); PRJM_EEL_FUNC1($$, "_gmem", $idx) }

/* Memory access via function */
| MEGABUF '(' expression[idx] ')'      { printf("[Parser] MEGABUF(INDEX) - MEM AT INDEX <EXP>\n"); PRJM_EEL_FUNC1($$, "_mem", $idx) }
| GMEGABUF '(' expression[idx] ')'     { printf("[Parser] GMEGABUF(INDEX) - GLOBAL MEM AT INDEX <EXP>\n"); PRJM_EEL_FUNC1($$, "_gmem", $idx) }
;

lvalue:
  variable                        { printf("[Parser] VAR lvalue\n"); $$ = $1; }
| memory                          { printf("[Parser] MEMORY ACCESS\n"); $$ = $1; }
;

variable:
  VAR                            { printf("[Parser] VAR: %s\n", $1); $$ = prjm_eel_compiler_create_variable(cctx, $1); }
;

empty-expression:
  %empty         { printf("[Parser] EMPTY EXPRESSION\n"); }
;

/* General expressions */
expression:
/* Literals */
  NUM                             { printf("[Parser] NUM: %g\n", $1); $$ = prjm_eel_compiler_create_const_expression(cctx, $1); }
| lvalue                          { printf("[Parser] LVALUE LITERAL\n"); $$ = $1; }

/* Compund assignment operators */
| lvalue[left] ADDOP expression[right]     { printf("[Parser] OPERATOR +=\n"); PRJM_EEL_FUNC2($$, "_addop", $left, $right) }
| lvalue[left] SUBOP expression[right]     { printf("[Parser] OPERATOR -=\n"); PRJM_EEL_FUNC2($$, "_subop", $left, $right) }
| lvalue[left] MODOP expression[right]     { printf("[Parser] OPERATOR %%=\n"); PRJM_EEL_FUNC2($$, "_modop", $left, $right) }
| lvalue[left] OROP expression[right]      { printf("[Parser] OPERATOR |=\n"); PRJM_EEL_FUNC2($$, "_orop", $left, $right) }
| lvalue[left] ANDOP expression[right]     { printf("[Parser] OPERATOR &=\n"); PRJM_EEL_FUNC2($$, "_andop", $left, $right) }
| lvalue[left] DIVOP expression[right]     { printf("[Parser] OPERATOR /=\n"); PRJM_EEL_FUNC2($$, "_divop", $left, $right) }
| lvalue[left] MULOP expression[right]     { printf("[Parser] OPERATOR /=\n"); PRJM_EEL_FUNC2($$, "_mulop", $left, $right) }
| lvalue[left] POWOP expression[right]     { printf("[Parser] OPERATOR ^=\n"); PRJM_EEL_FUNC2($$, "_powop", $left, $right) }

/* Comparison operators */
| expression[left] EQUAL expression[right]     { printf("[Parser] OPERATOR ==\n"); PRJM_EEL_FUNC2($$, "_equal", $left, $right) }
| expression[left] BELEQ expression[right]     { printf("[Parser] OPERATOR <=\n"); PRJM_EEL_FUNC2($$, "_beleq", $left, $right) }
| expression[left] ABOEQ expression[right]     { printf("[Parser] OPERATOR >=\n"); PRJM_EEL_FUNC2($$, "_aboeq", $left, $right) }
| expression[left] NOTEQ expression[right]     { printf("[Parser] OPERATOR !=\n"); PRJM_EEL_FUNC2($$, "_noteq", $left, $right) }
| expression[left] '<' expression[right]       { printf("[Parser] OPERATOR <\n"); PRJM_EEL_FUNC2($$, "_below", $left, $right) }
| expression[left] '>' expression[right]       { printf("[Parser] OPERATOR >\n"); PRJM_EEL_FUNC2($$, "_above", $left, $right) }

/* Boolean operators */
| expression[left] BOOLOR expression[right]    { printf("[Parser] OPERATOR ||\n"); PRJM_EEL_FUNC2($$, "bor", $left, $right) }
| expression[left] BOOLAND expression[right]   { printf("[Parser] OPERATOR &&\n"); PRJM_EEL_FUNC2($$, "band", $left, $right) }

/* Assignment operator */
| lvalue[left] '=' expression[right]           { printf("[Parser] ASSIGN expression TO LVALUE\n"); PRJM_EEL_FUNC2($$, "_set", $left, $right) }

| function                        { printf("[Parser] CALL FUNCTION\n"); $$ = $1; }

/* Ternary operator */
| expression[cond] '?' expression[trueval] ':' expression[falseval]   { printf("[Parser] TERNARY OPERATOR\n"); PRJM_EEL_FUNC3($$, "_if", $cond, $trueval, $falseval) }

/* Binary operators */
| expression[left] '+' expression[right] { printf("[Parser] OPERATOR +\n");  PRJM_EEL_FUNC2($$, "_add", $left, $right) }
| expression[left] '-' expression[right] { printf("[Parser] OPERATOR -\n");  PRJM_EEL_FUNC2($$, "_sub", $left, $right) }
| expression[left] '*' expression[right] { printf("[Parser] OPERATOR *\n");  PRJM_EEL_FUNC2($$, "_mul", $left, $right) }
| expression[left] '/' expression[right] { printf("[Parser] OPERATOR /\n");  PRJM_EEL_FUNC2($$, "_div", $left, $right) }
| expression[left] '%' expression[right] { printf("[Parser] OPERATOR %%\n"); PRJM_EEL_FUNC2($$, "_mod", $left, $right) }
| expression[left] '^' expression[right] { printf("[Parser] OPERATOR ^\n");  PRJM_EEL_FUNC2($$, "pow", $left, $right) }

/* Unary operators */
| '-' expression[value] %prec NEG        { printf("[Parser] UNARY -\n"); PRJM_EEL_FUNC1($$, "_neg", $value) }
| '+' expression[value] %prec POS        { printf("[Parser] UNARY +\n"); $$ = $value; } /* a + prefix does nothing. */
| '!' expression[value]                  { printf("[Parser] UNARY !\n"); PRJM_EEL_FUNC1($$, "_not", $value) }

/* Parenthesed expression */
| parentheses[value]                     { printf("[Parser] PARENTHESED EXP\n"); $$ = $value; }
;

/* End of grammar. */
%%
