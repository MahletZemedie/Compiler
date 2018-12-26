
#include "input_token.h"
#include "error.h"
#include <stdio.h>


int bufferExists = 0;
int buffer; // a non-Operand buffer

void get_operand(int *w) {
int c;

*w = 0; 
while ((c = getc(fpi)) != EOF)
   if (c >= '0' && c <= '9') *w = 10*(*w) + c - 48;
   else {ungetc(c,fpi); return;} 

}

int get_token(int *w) {
int c;

if (bufferExists) {bufferExists = 0; return buffer;}

while ((c = getc(fpi)) != EOF)
   if      ( c=='(' )                                         return(OpenBracket);
   else if ( c==')' )                                         return(CloseBracket);
   else if ( c=='+' )                                         return(Plus);
   else if ( c=='-' )                                         return(Minus);
   else if ( c=='*' )                                         return(Mult);
   else if ( c=='/' )                                         return(Divide);
   else if ( c=='^' )                                         return(Expon);
   else if ( c>='0' && c<='9' ) {ungetc(c,fpi);get_operand(w);return(Operand);}
   else if ( c>=33  && c<=126 ) error(1);

return(EOF);
}

void unget_token(int token_returned) {
   buffer = token_returned;
   bufferExists = 1;
}