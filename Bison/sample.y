%{
    #include <stdio.h>
    #include <math.h>
    void yyerror(char const *);
%}

%union {
	int iType;
	double dType;
	char cType;
}

%token <iType> NUM
%type <iType> exp
%left '-' '+'
%left '*' '/'
%right '^'

%%

input :
      | input line
;

line : '\n'
     | exp '\n'       {printf("\t=> %d\n", $1);}
;

exp  : NUM            {$$ = $1;}
     | '(' exp ')'    {$$ = $2;}
     | exp '+' exp    {$$ = $1 + $3;}
     | exp '-' exp    {$$ = $1 - $3;}
     | exp '*' exp    {$$ = $1 * $3;}
     | exp '/' exp    {$$ = $1 / $3;}
     | exp '^' exp    {$$ = pow($1, $3)}
;
%%

int main() {
	return yyparse();
}

void yyerror(char const *s) {
	fprintf(stderr, "%s\n", s);
}