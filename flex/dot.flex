%{
   #include "stdio.h"
   int flag = 0;
%}

KEYWORD     (?i:STRICT)|(?i:GRAPH)|(?i:DIGRAPH)|(?i:SUBGRAPH)|(?i:NODE)|(?i:EDGE)
NUMBER      [+-]*[0-9]+(\.)[0-9]+|[-+]*[0-9]+
ID          [A-Za-z_][A-Za-z0-9_]*
STRING      [\"][^\"]*[\"] 

SEPERATOR   [;,\{\}\[\]]
SIGN        (--)|(->)|(=)

NOTATION    #[^\n]*(\n)*|(\/\/)[^\n]*(\n)*
ENTER       (\n)*
SPACE       (" "|\t)*

%x   comment

%%

{KEYWORD} {
	printf("%s\n", yytext);
}

{SIGN} {
    printf("%s\n", yytext);	
}

{NUMBER}|{ID} {
	printf("%s\n", yytext);
}

{STRING} {
    // 解析到字符串里面包含转义字符“
	if (yytext[yyleng-2] == '\\') {
	    int i;
	    // 刚开始解析字符串要从第0个字符(")输出，否则从第1个字符输出
	    if (flag == 0) {
	        i = 0;
	    } else {
	        i = 1;
	    }
	    for (; i < yyleng; i++) {
	         printf("%c", yytext[i]);
	    }
	    yyless(yyleng-1);
	    flag = 1;
	} else if (flag == 1) {
	    int i;
	    for (i = 1; i < yyleng; i++) {
	         printf("%c", yytext[i]);
	    }
	    printf("\n");
	    flag = 0;
	} else {
	    printf("%s\n", yytext);
	}
	
}


{SEPERATOR} {
	printf("%s\n", yytext);
}


{NOTATION}|{ENTER}|{SPACE} {
	
}

"/*"    BEGIN(comment);
<comment>[^*\n]*        {}
<comment>"*"+[^*/\n]*   {}
<comment>\n             {}
<comment>"*"+"/"        BEGIN(INITIAL);


%%
