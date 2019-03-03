
%{

#include <stdio.h>

/* need this for the call to atoi() and atof() below */
#include <math.h>

%}

quote                    \"[^"]*\"


digit                    [0-9]
letter                   [A-Za-z_]
f1                       ({digit}+".")*
f2                       ({digit}+"."{digit})*
f3                       ("."+{digit}+)*
E1                       ([Ee]{digit})*
E2                       ([Ee][+-]{digit}*)*

ID                        {letter}({letter}|{digit})*
int                       {digit}+
float                     ({f1}|{f2}|{f2}{E1}|{f2}{E2}|{f3}|{f3}{E1}|{f3}{E2})
singleLineComment         ("//"[^\n]*"\n")|"//"([^\n]*\\[ \t]*"\n")*[^\n]*"\n"
multiLineComment           "/*"([^*]|\*+[^*/])*\*+"/"                     
comment                   ({singleLineComment}|{multiLineComment})


%%

{int}                    {
                         printf("An integer:                    %s (%d)\n", 
                                yytext, atoi(yytext));
                         }

{float}                  {
                         printf("A float:                       %s (%g)\n", 
                                yytext, atof(yytext));
                         }

void|break|continue|else|float|if|int|return|while {
                         printf("A keyword:                     %s\n", 
                                 yytext); }

{ID}                     {
                         printf("An identifier:                 %s\n", 
                                 yytext); }

"+"|"-"|"*"|"/"|"%"|"!"|"?"|":"|"="|","|"<"|">"|";"|"("|")"|"{"|"}"  {
                         printf("operator/bracket/other char:   %s\n",
                                 yytext); }

"||"                     printf("Disjunction operator:          %s\n", yytext);

"&&"                     printf("Conjunction operator:          %s\n", yytext);

"=="                     printf("Equality operator:             %s\n", yytext);

{comment}                {
                         printf("Comment:                       %s\n", yytext);
                         }

[ \t\n]+                 /* eat up white space */ ;

.                        printf("Unrecognized character:        %s\n", yytext);

%%

int main()
{
                         yylex();
}
