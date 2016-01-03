%{
   #include "stdio.h"	
   //int num = 1;
%}

ENTER   (\n)*
SAME    (25[0-5]|2[0-4][0-9]|1[0-9]{2}|[1-9][0-9]{1}|[0-9])

IPA    ^(1[0-2]{1}[0-7]{1}|[1-9][0-9]|[0-9])(\.){SAME}(\.){SAME}(\.){SAME}$
IPB    ^(1[2-8][0-9]|19[0-1])(\.){SAME}(\.){SAME}(\.){SAME}$
IPC    ^(19[2-9]|2[0-1][0-9]|22[0-3])(\.){SAME}(\.){SAME}(\.){SAME}$
IPD    ^(22[4-9]|23[0-9])(\.){SAME}(\.){SAME}(\.){SAME}$
IPE    ^(24[0-9]|25[0-5])(\.){SAME}(\.){SAME}(\.){SAME}$

OTHER  .*

%%

{IPA} {
    //printf("%d: ", num++);
	printf("A\n");
}

{IPB} {
    //printf("%d: ", num++);
	printf("B\n");
}

{IPC} {
    //printf("%d: ", num++);
	printf("C\n");
}

{IPD} {
    //printf("%d: ", num++);
	printf("D\n");
}

{IPE} {
    //printf("%d: ", num++);
	printf("E\n");
}

{OTHER} {
    //printf("%d: ", num++);
	printf("Invalid\n");
}

{ENTER} {
    if (yyleng >= 2) {
        int i;
        for (i = 1; i < yyleng; i++) {
             //printf("%d: ", num++);
             printf("Invalid\n");
        }
    }
	/*printf("%ld, %s", yyleng, yytext);*/
}

%%