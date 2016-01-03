%{
   #include "stdio.h"
   #include "stdlib.h"	

   int flag = 0;
   int line_num = 1;
%}

%x comment

%%

"/*"   BEGIN(comment);

<comment>[^*\n]*      {}
<comment>"*"+[^*/\n]*   {}
<comment>\n        { ++line_num;
                     printf("%d\n", line_num);}
<comment>"*"+"/"    BEGIN(INITIAL);

%%
