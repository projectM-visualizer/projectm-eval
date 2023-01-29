%code requires {

  #include "CompilerTypes.h"
  #include "Compiler.h"

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
%token <char*> NAME
%nterm <prjm_eel_compiler_node_t*> variable function function-arglist memory lvalue program instruction-list expression

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
  instruction-list { printf ("PROGRAM END\n"); }
;

/* Functions */
function:
  NAME '(' function-arglist ')'            { printf("FUNCTION: %s\n", $1); }
;

function-arglist:
  instruction-list                         { printf("FUNCTION ARGLIST START\n"); }
| function-arglist ',' instruction-list    { printf("FUNCTION ARGLIST +ARG\n"); }
;

parentheses:
  '(' instruction-list ')'                 { printf("PARENTHESED EXPRESSION\n"); }
| '(' ')'                                  { printf("EMPTY PARENTHESES\n"); }
;

instruction-list:
  expression                               { printf ("INSTRUCTION LIST START\n"); }
| instruction-list ';' expression          { printf ("INSTRUCTION LIST +1\n"); }
| instruction-list ';' empty-expression    { printf ("INSTRUCTION LIST +0\n"); }
| instruction-list ';' error               { printf ("INSTRUCTION LIST ERROR +0\n"); }
;

/* Memory access */
memory:
/* Memory access via index */
  expression '[' ']'                    { printf("INDEX[] - MEM AT INDEX <EXP>\n"); }
| expression '[' expression ']'         { printf("INDEX[OFFSET] - MEM AT INDEX <EXP> WITH OFFSET <EXP>\n"); }
| GMEM '[' expression ']'               { printf("GMEM[INDEX] - GLOBAL MEM AT INDEX <EXP>\n"); }

/* Memory access via function */
| MEGABUF '(' expression ')'      { printf("MEGABUF(INDEX) - MEM AT INDEX <EXP>\n"); }
| GMEGABUF '(' expression ')'     { printf("GMEGABUF(INDEX) - GLOBAL MEM AT INDEX <EXP>\n"); }
;

lvalue:
  variable
| memory                          { printf("MEMORY ACCESS\n"); }
;

variable:
  NAME                            { printf("VAR: %s\n", $1); }
;

empty-expression:
  %empty         { printf("EMPTY EXPRESSION\n"); }
;

/* General expressions */
expression:
/* Literals */
  NUM                             { printf("NUM: %g\n", $1); }
| lvalue                          { printf("LVALUE\n", $1); }

/* Compund assignment operators */
| lvalue ADDOP expression     { printf("OPERATOR +=\n"); }
| lvalue SUBOP expression     { printf("OPERATOR -=\n"); }
| lvalue MODOP expression     { printf("OPERATOR %%=\n"); }
| lvalue OROP expression      { printf("OPERATOR |=\n"); }
| lvalue ANDOP expression     { printf("OPERATOR &=\n"); }
| lvalue DIVOP expression     { printf("OPERATOR /=\n"); }
| lvalue MULOP expression     { printf("OPERATOR /=\n"); }
| lvalue POWOP expression     { printf("OPERATOR ^=\n"); }

/* Comparison operators */
| expression EQUAL expression     { printf("OPERATOR ==\n"); }
| expression BELEQ expression     { printf("OPERATOR <=\n"); }
| expression ABOEQ expression     { printf("OPERATOR >=\n"); }
| expression NOTEQ expression     { printf("OPERATOR !=\n"); }
| expression '<' expression       { printf("OPERATOR <\n"); }
| expression '>' expression       { printf("OPERATOR >\n"); }

/* Boolean operators */
| expression BOOLOR expression    { printf("OPERATOR ||\n"); }
| expression BOOLAND expression   { printf("OPERATOR &&\n"); }

/* Assignment operator */
| lvalue '=' expression           { printf("ASSIGN expression TO LVALUE\n"); }

| function                        { printf("CALL FUNCTION\n"); }

/* Ternary operator */
| expression '?' expression ':' expression   { printf("TERNARY OPERATOR\n"); }

/* Binary operators */
| expression '+' expression       { printf("OPERATOR +\n"); }
| expression '-' expression       { printf("OPERATOR -\n"); }
| expression '*' expression       { printf("OPERATOR *\n"); }
| expression '/' expression       { printf("OPERATOR /\n"); }
| expression '%' expression       { printf("OPERATOR %%\n");}
| expression '^' expression       { printf("OPERATOR ^\n"); }

/* Unary operators */
| '-' expression %prec NEG        { printf("UNARY -\n"); }
| '+' expression %prec POS        { printf("UNARY +\n");  }
| '!' expression                  { printf("UNARY !\n"); }

/* Parenthesed expression */
| parentheses                     { printf("PARENTHESED EXP\n"); }
;

/* End of grammar. */
%%
