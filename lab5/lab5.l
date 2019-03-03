%{
#include <stdio.h>
%}

%%

"/"                      {printf("l");}
\\                       {printf("b");}
[ \t]                    {printf("w");}
\n                       {printf("n");}
[^/ \t\n\\]              {printf("a");}

%%