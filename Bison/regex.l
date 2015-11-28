%{
    #include <stdlib.h>
	#include "regex.tab.h"
%}

%%

"|"     return '|';
"*"     return '*';
"+"     return '+';
"?"     return '?';
"("     return '(';
")"     return ')';
":"     return ':';
"\n"    return '\n';
.       { yylval.cType = yytext[0];  return CHAR; }

%%

int yywrap(void) {
	return 1;
}