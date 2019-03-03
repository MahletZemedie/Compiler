
#include "stack.h"
#include <stdlib.h>

struct stack_struct {
   int opn;
   struct stack_struct *next;
};

stack new_stack()
{stack t;
   t = (struct stack_struct *) malloc( sizeof( struct stack_struct ) );  
   t->next = NULL;
   return(t);
}

void push(stack s, int opn)
{stack t;
   t = (struct stack_struct *) malloc( sizeof( struct stack_struct ) );  
   t->opn  = opn;
   t->next = s->next;
   s->next = t;
}

int is_empty_stack(stack s)
{
   return( s->next == NULL );
}

int pop(stack s) 
{
   int opn = s->next->opn;
   s->next = s->next->next;
   return opn;
}
int top(stack stk)
{
int t=stk->next->opn;
return t;

}


