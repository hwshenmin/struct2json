
%{
#include "structjson_lib.h"
%}

%union {
    member_t *mbr;
    struct_t *srt;
    module_t *mod;
    symbol_t *sym;
    type_t *typ;
}

%token MODULE
%token STRUCT
%token OPTIONAL
%token REQUIRE
%token INT
%token UNSIGNED
%token VECTOR
%token STRING

%token NUMBER
%token <sym> IDENTIFIER

%token LEFTBRACE
%token RIGHTBRACE
%token LEFTANGLE
%token RIGHTANGLE
%token SIMICOLON

%type <mod> module
%type <srt> structlist struct
%type <mbr> statementlist statement
%type <typ> type simpletype

%start module

%%
module: MODULE IDENTIFIER LEFTBRACE structlist RIGHTBRACE SIMICOLON {return translate(print_module(new_module($2, $4)));}
      | MODULE IDENTIFIER LEFTBRACE structlist RIGHTBRACE {return translate(print_module(new_module($2, $4)));}
      ;
structlist: {$$ = NULL;}
          |structlist struct { $$ = merge_struct($1, $2);}
          ;
struct: STRUCT IDENTIFIER LEFTBRACE statementlist RIGHTBRACE SIMICOLON {$$ = new_struct($2, $4);}
      ;
statementlist: {$$ = NULL;}
             | statementlist statement {$$ = merge_member($1, $2);}
             ;
statement: NUMBER OPTIONAL type IDENTIFIER SIMICOLON {$$ = new_member($3, $4);}
         | NUMBER REQUIRE type IDENTIFIER SIMICOLON {$$ = new_member($3, $4);}
         | type IDENTIFIER SIMICOLON {$$ = new_member($1, $2);}
         ;
type: simpletype {$$ = $1;}
    | VECTOR LEFTANGLE simpletype RIGHTANGLE {$3->vectype = $3->type; $3->type = TYPE_VECTOR; $$ = $3;}
    ;
simpletype: INT {$$ = new_type(TYPE_INT, 0, new_symbol("int"));}
    | UNSIGNED INT {$$ = new_type(TYPE_UINT, 0, new_symbol("unsigned int"));}
    | STRING {$$ = new_type(TYPE_STRING, 0, new_symbol("string"));}
    | IDENTIFIER {$$ = new_type(TYPE_CUSTOM, 0, $1);}
    ;
%%
#if 0
int main(int argc, char *argv)
{
    yyparse();
    return 0;
}
#endif
extern int yylineno;
void yyerror(char *s, ...)
{
    va_list ap;
    va_start(ap, s);
    vfprintf(stderr, s, ap);
    va_end(ap);
	fprintf(stderr, "\nlineno=%d\n", yylineno);
}
