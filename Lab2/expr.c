
#include <math.h>
#include <stdlib.h>
#include <stdio.h> 
#include "input_token.h"
#include "error.h"
#include "bTree.h"

btree eval_expr_5(btree opn1,int opr1, btree opn2, int opr2, btree opn3);
btree eval_expr_4(btree opn1,int opr1, btree opn2, int opr2);
btree eval_expr_3(btree opn1,int opr1, btree opn2);
btree eval_expr_2(btree opn1,int opr1);  
btree eval_expr_1(btree opn1);  
btree eval_expr();  

btree eval_expon_1(btree opn1);  
btree eval_expon();

btree evaluate_expression() {  
   int w; 
   btree val;
   if (get_token(&w)!=OpenBracket) error(-1);
   val =  eval_expr();
   if (get_token(&w)!=EOF) error(-2);
   return val;
}

btree eval_expr()
/* have seen

        (

   read until we find a matching parenthesis.
   the expression enclosed is evaluated.
   return the integer value computed.
*/
{int w;
    switch (get_token(&w))
    { case OpenBracket:
           return(eval_expr_1(eval_expr()));
      case CloseBracket:
           error(2);
      case Plus:
           error(3);
      case Minus:
           error(4);
      case Mult:
           error(5);
      case Divide:
           error(6);
      case Expon:
           error(100);
      case Operand:
           return(eval_expr_1(MakeBTree(NULL,w,NULL)));
      case EOF:
           error(7);
    }
}

btree eval_expr_1(btree opn1)
/* have seen

        ( opn1

   where opn1 is an operand

   read until we find a matching parenthesis.
   the expression enclosed is evaluated.
   return the integer value computed.
*/
{btree w;
    switch (get_token(&w))
    { case OpenBracket:
           error(8);
      case CloseBracket:
           return(opn1);
      case Plus:
           return(eval_expr_2(opn1,Plus));
      case Minus:
           return(eval_expr_2(opn1,Minus));
      case Mult:
           return(eval_expr_2(opn1,Mult));
      case Divide:
           return(eval_expr_2(opn1,Divide));
      case Expon:
	   //return(eval_expr_1(pow(opn1,eval_expon())));
	   	   return(eval_expr_1(MakeBTree(opn1,Expon,eval_expon())));
      case Operand:
           error(10);
      case EOF:
           error(11);
    }
}

btree eval_expr_2(btree opn1,int opr1)
/* have seen

        ( opn1 opr1

   where opn1 is an operand
     and opr1 is an operator

   read until we find a matching parenthesis.
   the expression enclosed is evaluated.
   return the integer value computed.
*/
{int w;
    switch (get_token(&w))
    { case OpenBracket:
           return(eval_expr_3(opn1,opr1,eval_expr()));
      case CloseBracket:
           error(12);
      case Plus:
           error(13);
      case Minus:
           error(14);
      case Mult:
           error(15);
      case Divide:
           error(16);
      case Expon:
           error(101);
      case Operand:
           return(eval_expr_3(opn1,opr1,MakeBTree(NULL,w,NULL)));
      case EOF:
           error(17);
    }
}



btree eval_expr_3(btree opn1,int opr1, btree opn2)
/* have seen

        ( opn1 opr1 opn2

   where opn1 and opn2 are operands, 
         opr1 is an operator

   read until we find a matching parenthesis.
   the expression enclosed is evaluated.
   return the integer value computed.
*/
{int w;
    switch (get_token(&w))
    { case OpenBracket:
           error(18);
      case CloseBracket:
           switch (opr1) 
           { //case Plus:   return(opn1+opn2);
		     case Plus:   return(MakeBTree(opn1,Plus,opn2));
             case Minus:  return(MakeBTree(opn1,Minus,opn2));
             case Mult:   return(MakeBTree(opn1,Mult,opn2));
             case Divide: return(MakeBTree(opn1,Divide,opn2));
           }
      case Plus:
           switch (opr1)
           { case Plus:   return(eval_expr_2(MakeBTree(opn1,Plus,opn2),Plus));  //return(eval_expr_2(opn1+opn2,Plus));
             case Minus:  return(eval_expr_2(MakeBTree(opn1,Minus,opn2),Plus)); //return(eval_expr_2(opn1-opn2,Plus));
             case Mult:   return(eval_expr_2(MakeBTree(opn1,Mult,opn2),Plus)); //return(eval_expr_2(opn1*opn2,Plus));
             case Divide: return(eval_expr_2(MakeBTree(opn1,Divide,opn2),Plus));//return(eval_expr_2(opn1/opn2,Plus));
           }
      case Minus:
           switch (opr1)
           { case Plus:  return(eval_expr_2(MakeBTree(opn1,Plus,opn2),Minus));  //return(eval_expr_2(opn1+opn2,Minus));
             case Minus: return(eval_expr_2(MakeBTree(opn1,Minus,opn2),Minus)); //return(eval_expr_2(opn1-opn2,Minus));
             case Mult:  return(eval_expr_2(MakeBTree(opn1,Mult,opn2),Minus)); // return(eval_expr_2(opn1*opn2,Minus));
             case Divide: return(eval_expr_2(MakeBTree(opn1,Divide,opn2),Minus));//return(eval_expr_2(opn1/opn2,Minus));
           }
      case Mult:
           switch (opr1)
           { case Plus:   return(eval_expr_4(opn1,opr1,opn2,Mult));
             case Minus:  return(eval_expr_4(opn1,opr1,opn2,Mult));
             case Mult:   return(eval_expr_2(MakeBTree(opn1,Mult,opn2),Mult));//return(eval_expr_2(opn1*opn2,Mult));
             case Divide: return(eval_expr_2(MakeBTree(opn1,Divide,opn2),Mult));//return(eval_expr_2(opn1/opn2,Mult));
           }
      case Divide:
           switch (opr1)
           { case Plus:   return(eval_expr_4(opn1,opr1,opn2,Divide));
             case Minus:  return(eval_expr_4(opn1,opr1,opn2,Divide));
             case Mult:   return(eval_expr_2(MakeBTree(opn1,Mult,opn2),Divide));//return(eval_expr_2(opn1*opn2,Divide));
             case Divide: return(eval_expr_2(MakeBTree(opn1,Divide,opn2),Divide));//return(eval_expr_2(opn1/opn2,Divide));
           }
      case Expon:
	   //return(eval_expr_3(opn1,opr1,pow(opn2,eval_expon())));  
	   return(eval_expr_3(opn1,opr1,MakeBTree(opn2,Expon,eval_expon())));
      case Operand:
           error(20);
      case EOF:
           error(21);
    }
}


btree eval_expr_4(btree opn1,int opr1, btree opn2, int opr2)
/* have seen

        ( opn1 opr1 opn2 opr2

   where opn1 and opn2 are operands,
         opr1 and opr2 are operators

   read until we find a matching parenthesis.
   the expression enclosed is evaluated.
   return the integer value computed.
*/
{int w;
    switch (get_token(&w))
    { case OpenBracket:
           return(eval_expr_5(opn1,opr1,opn2,opr2,eval_expr()));
      case CloseBracket:
           error(22);
      case Plus:
           error(23);
      case Minus:
           error(24);
      case Mult:
           error(25);
      case Divide:
           error(26);
      case Expon:
           error(102);
      case Operand:
          // return(eval_expr_5(opn1,opr1,opn2,opr2,w));
		   return(eval_expr_5(opn1,opr1,opn2,opr2,MakeBTree(NULL,w,NULL)));
      case EOF:
           error(27);
    }
}

btree eval_expr_5(btree opn1,int opr1, btree opn2, int opr2, btree opn3)

/* have seen

        ( opn1 opr1 opn2 opr2 opn3

   where opn1, opn2 and opn3 are operands,
         opr1 and opr2 are operators

   read until we find a matching parenthesis.
   the expression enclosed is evaluated.
   return the integer value computed.
*/
{int w;
    switch (get_token(&w))
    { case OpenBracket:
           error(201);
      case CloseBracket:
           switch (opr2) 
           { case Mult:   switch (opr1) 
                          { case Plus:  return(MakeBTree(opn1,Plus,MakeBTree(opn2,Mult,opn3))); // return(opn1+opn2*opn3);
                            case Minus: return(MakeBTree(opn1,Minus,MakeBTree(opn2,Mult,opn3))); //return(MakeTree(opn1,Minus,opn2,Mult,opn3));
                          }
             case Divide: switch (opr1) 
                          { case Plus:  return(MakeBTree(opn1,Plus,MakeBTree(opn2,Divide,opn3)));//return(opn1+opn2/opn3);
                            case Minus: return(MakeBTree(opn1,Minus,MakeBTree(opn2,Divide,opn3)));//(opn1-opn2/opn3);
                          }
           }
      case Plus:
           switch (opr2) 
           { case Mult:   switch (opr1) 
                          { case Plus:  return(eval_expr_2(MakeBTree(opn1,Plus,MakeBTree(opn2,Mult,opn3)),Plus)); //(eval_expr_2(opn1+opn2*opn3,Plus));
                            case Minus: return(eval_expr_2(MakeBTree(opn1,Minus,MakeBTree(opn2,Mult,opn3)),Plus));//(eval_expr_2(opn1-opn2*opn3,Plus));
                          }
             case Divide: switch (opr1) 
                          { case Plus:  return(eval_expr_2(MakeBTree(opn1,Plus,MakeBTree(opn2,Divide,opn3)),Plus));//(eval_expr_2(opn1+opn2/opn3,Plus));
                            case Minus: return(eval_expr_2(MakeBTree(opn1,Minus,MakeBTree(opn2,Divide,opn3)),Plus));//(eval_expr_2(opn1-opn2/opn3,Plus));
                          }
           }
      case Minus:
           switch (opr2) 
           { case Mult:   switch (opr1) 
                          { //case Plus:  return(eval_expr_2(opn1+opn2*opn3,Minus));
                            case Plus:  return(eval_expr_2(MakeBTree(opn1,Plus,MakeBTree(opn2,Mult,opn3)),Minus));
							case Minus: return(eval_expr_2(MakeBTree(opn1,Minus,MakeBTree(opn2,Mult,opn3)),Minus));//(eval_expr_2(opn1-opn2*opn3,Minus));
                          }
             case Divide: switch (opr1) 
                          { case Plus:  return(eval_expr_2(MakeBTree(opn1,Plus,MakeBTree(opn2,Divide,opn3)),Minus));//(eval_expr_2(opn1+opn2/opn3,Minus));
                            case Minus: return(eval_expr_2(MakeBTree(opn1,Minus,MakeBTree(opn2,Divide,opn3)),Minus));//(eval_expr_2(opn1-opn2/opn3,Minus));
                          }
           }
      case Mult:
           switch (opr2) 
           { case Mult:   return(eval_expr_4(opn1,opr1,MakeBTree(opn2,Mult,opn3),Mult));//(eval_expr_4(opn1,opr1,opn2*opn3,Mult));
             case Divide: return(eval_expr_4(opn1,opr1,MakeBTree(opn2,Divide,opn3),Mult));//(eval_expr_4(opn1,opr1,opn2/opn3,Mult));
           }
      case Divide:
           switch (opr2) 
           { case Mult:   return(eval_expr_4(opn1,opr1,MakeBTree(opn2,Mult,opn3),Divide));//(eval_expr_4(opn1,opr1,opn2*opn3,Divide));
             case Divide: return(eval_expr_4(opn1,opr1,MakeBTree(opn2,Divide,opn3),Divide));//(eval_expr_4(opn1,opr1,opn2/opn3,Divide));
           }
      case Expon:
           return(eval_expr_5(opn1,opr1,opn2,opr2,MakeBTree(opn3,Expon,eval_expon()))); // (eval_expr_5(opn1,opr1,opn2,opr2,pow(opn3,eval_expon())));
      case Operand:
           error(202);
      case EOF:
           error(203);
    }
}

btree eval_expon() 
/*
   have just seen an exponentiation operator ^

   read until an operator of lower priority 
   or a closing parenthesis or EOF is encountered.

   observe the right associate law of exponentiation.

   return the exponent value.
*/
{int w;
    switch (get_token(&w))
    { case OpenBracket:
           return(eval_expon_1(eval_expr()));
      case CloseBracket:
           error(2);
      case Plus:
           error(3);
      case Minus:
           error(4);
      case Mult:
           error(5);
      case Divide:
           error(6);
      case Expon:
           error(103);
      case Operand:
           return(eval_expon_1(MakeBTree(NULL,w,NULL)));
      case EOF:
           error(7);
    }
}

btree eval_expon_1(btree opn1) 
/*
   same context as for eval_expon()

   just seen an operand, and before that an exponentiation ^ operator

*/
{int w;
    switch (get_token(&w))
    { case OpenBracket:
           error(104);
      case CloseBracket:
           unget_token(CloseBracket);
           return(opn1);
      case Plus:
           unget_token(Plus);
           return(opn1);
      case Minus:
           unget_token(Minus);
           return(opn1);
      case Mult:
           unget_token(Mult);
           return(opn1);
      case Divide:
           unget_token(Divide);
           return(opn1);
      case Expon:
           return (MakeBTree(opn1,Expon,eval_expon()));  //(pow(opn1,eval_expon()));
      case Operand:
           error(105);
      case EOF:
           error(106);
    }
}