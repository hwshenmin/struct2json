%{
#include "structjson_lib.h"
#include "structjson_bison.h"
%}
%option yylineno
%%
"module"        { return MODULE; }
"namespace"        { return MODULE; }
"struct"        { return STRUCT; }
"int"           { return INT; }
"unsigned"      { return UNSIGNED; }
"string"        { return STRING; }
"vector"        { return VECTOR; }
"optional"      { return OPTIONAL; }
"require"       { return REQUIRE; }

[0-9]+        { return NUMBER; }
[a-zA-Z_][a-zA-Z0-9_]* {yylval.sym = new_symbol(yytext); return IDENTIFIER; }
[ \t\r\n]       { }

"{"             { return LEFTBRACE; }
"}"             { return RIGHTBRACE; }
"<"             { return LEFTANGLE; }
">"             { return RIGHTANGLE; }
";"             { return SIMICOLON; }
"//".* 

.               { fprintf(stderr, "unrecognized char: %c\n", *yytext);exit(1);}

%%
#if 0
int main(int argc, char **argv)
{
    int tok;
    while ((tok = yylex()))
    {
        printf("token: %d\n", tok);
    }
    return 0;
}
#endif
