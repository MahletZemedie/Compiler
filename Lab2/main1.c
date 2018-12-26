#include "stack.h"
#include <stdlib.h>
#include "input_token.h"
#include "error.h"
#include <stdio.h>
#include <math.h>


int main(int argc, char *argv[]){
 
   fpi=stdin;
   
   int val1;
   int val2;
   int w;
   int x;
stack s=  new_stack();
	
 while ((x = get_token(&w)) != EOF){
	 switch(x){
		 case Operand: 
		      push(s,w);
		      break;
		  case Plus: 
		      val1=pop(s);
			  val2=pop(s);
			   push(s,val2+val1);
			   
		       break;
		   case Minus: 
		       val1=pop(s);
			   val2=pop(s);
			   push(s,val2-val1);
		       break;
		   case Mult: 
		      val1=pop(s);
			  val2=pop(s);
			   push(s,val2*val1);
		       break;	  
           case Divide: 
		      val1=pop(s);
			  val2=pop(s);
			   push(s,val2/val1);
		       break;	
          case Expon: 
		      val1=pop(s);
			  val2=pop(s);
			   push(s,pow(val2,val1));
		       break;				   
		
		default:
		    printf("...not functioning well %d");
			break;
	
	 
 }}
        printf("expression result =  % d", pop(s)); 
		
 
 }
 
 
 

