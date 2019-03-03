
%{

#include <math.h>
int INTdenotation = 1;
int plus = 2;
int mult = 3;
int expon= 4;
int openP= 5;
int closeP=6;
int yylval;

%}

delim          [ \t\n]
ws             {delim}+
digit          [0-9]
int            {digit}+

%%

{ws}           {/* no action and no return */}
{int}          {yylval = atoi(yytext); return(INTdenotation);}
[+]            {return(plus);}
[*]            {return(mult);}
[\^]            {return(expon);}
[(]            {return(openP);}
[)]            {return(closeP);}
'\n'           {}
.              {} 

%%


