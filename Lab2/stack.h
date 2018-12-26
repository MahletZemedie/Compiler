
typedef struct stack_struct *stack;

stack new_stack();
/* return a new empty stack */

void push(stack s, int opn);
/* push "item" into the stack s */

int pop(stack s); // assume stack is not empty
/* return top operand, and pop away the top entry of stack s */

int is_empty_stack(stack s);
/* return 1 if the stack s is empty, and return 0 otherwise */
