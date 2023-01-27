%code top {
  #include "Calc.h"   /* Contains definition of 'symrec'. */
  #include "Compiler.h"
  int prjm_eel_lex (YYSTYPE * yylval_param, YYLTYPE * yylloc_param );
  void prjm_eel_error (char const *);
}

/* Generator options */
%verbose
%locations
%no-lines
%define api.pure true
%define api.prefix {prjm_eel_}
%define api.header.include {"Calc.h"}
%define api.value.type union /* Generate YYSTYPE from these types: */
%define parse.error verbose

/* Token declarations */

/* Special functions */
%token LOOP WHILE GMEM GMEGABUF MEGABUF

/* Operator tokens */
%token ADDOP SUBOP MODOP OROP ANDOP DIVOP MULOP POWOP EQUAL BELEQ ABOEQ NOTEQ BOOLOR BOOLAND

/* Value types */
%token <float>  NUM     /* Simple number. */
%token <symrec*> VAR FUN /* Symbol table pointer: variable/function. */
%nterm <symrec*> function memory lvalue
%nterm <float> program instruction-list loop instruction expression

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
%precedence NEG POS /* unary minus or plus */
%left '^'

%% /* The grammar follows. */

program:
  instruction-list { printf ("PROGRAM END\n"); }
;

instruction-list:
  instruction                  { printf ("INSTRUCTION LIST START\n"); }
| instruction-list instruction { printf ("INSTRUCTION LIST +1\n"); }
;

instruction:
  expression ';'    { printf ("INSTRUCTION EXPRESSION\n"); }
| loop ';'          { printf ("INSTRUCTION LOOP\n"); }
;

/**
 * loop() and while() are special, as these are the only "functions" which takes a list
 * of expressions as the second argument. This isn't allowed in any other function.
 */
loop:
  LOOP '(' expression ',' instruction-list ')' { printf ("LOOP END\n"); }
| WHILE '(' expression ',' instruction-list ')' { printf ("WHILE END\n"); }
;

/* Functions */
function:
  FUN '(' expression ')'                                 { printf("FUNC 1: %s\n", $1->name); }
| FUN '(' expression ',' expression ')'                  { printf("FUNC 2: %s\n", $1->name); }
| FUN '(' expression ',' expression ',' expression ')'   { printf("FUNC 3: %s\n", $1->name); }
;

/* Memory access */
memory:
/* Memory access via index */
  expression '[' ']'              { printf("INDEX[] - MEM AT INDEX <EXP>\n"); }
| expression '[' expression ']'   { printf("INDEX[OFFSET] - MEM AT INDEX <EXP> WITH OFFSET <EXP>\n"); }
| GMEM '[' expression ']'         { printf("GMEM[INDEX] - GLOBAL MEM AT INDEX <EXP>\n"); }

/* Memory access via function */
| MEGABUF '(' expression ')'      { printf("MEGABUF(INDEX) - MEM AT INDEX <EXP>\n"); }
| GMEGABUF '(' expression ')'     { printf("GMEGABUF(INDEX) - GLOBAL MEM AT INDEX <EXP>\n"); }
;

lvalue:
  VAR                             { printf("VAR: %s\n", $1->name); }
| memory                          { printf("MEMORY ACCESS\n"); }
;

/* General expressions */
expression:
  %empty                          { printf("EMPTY EXPRESSION\n"); }
/* Literals */
| NUM                             { printf("NUM: %g\n", $1); }
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
| '(' expression ')'              { printf("PARENTHESED EXP\n"); }
;

/* End of grammar. */
%%
