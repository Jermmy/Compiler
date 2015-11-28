%{
    #include <stdlib.h>
    #include "sample.tab.h"
%}

%%

[0-9]+        {yylval.iType = atoi(yytext); return NUM;}
"+"           return '+';
"-"           return '-';
"*"           return '*';
"/"           return '/';
"^"           return '^';
"("           return '(';
")"           return ')';
"\n"          return '\n';
[ \t]+        ;

.             yyerror("Unknown character");

%%

int yywrap(void) {
	return 1;
}