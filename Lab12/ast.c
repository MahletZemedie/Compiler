/**  ast.c
 **
 **     The Abstract Syntax Tree module
 **/

#include <stdio.h>
#include "symtbl.h"
#include "ast.h"
#include "mem.h"

#define AST_POOL_SIZE 1024

/* global variables */
ASTNODE *ast_root;

/* local variables */
char *asttypename[37] =
{
    "PROGRAM",
    "PARA_LIST",
    "PARA",
    "LIST",
    "IF_ELSE",
    "IF_THEN",
    "TWO_SIDED",
    "WHILE",
    "BREAK",
    "CONTINUE",
    "RETURN",
    "ASSIGN",
    "ID",
    "COND_EXP",
    "ALT",
    "OR",
    "AND",
    "EQ",
    "LT",
    "GT",
    "ADD",
    "SUB",
    "MUL",
    "DIV",
    "MOD",
    "NOT",
    "UMINUS",
    "INT",
    "FLOAT",
    "PROGRAM_LIST",
    "EXPR_LIST",
    "EXPR",
    "FUNC_CALL",
    "ARRAY_INDEX",
    "ARRAY_REF",
    "ARRAY_DECL",
    "ARRAY_DIMS"
};


MEM_POOL ast_pool;
int      ast_node_num;

/* Initialize the ast module - allocate pool for ast */
void ast_init()
{
    ast_pool = mem_alloc_pool(AST_POOL_SIZE*sizeof(ASTNODE));
    ast_node_num = 0;
}

/* Create AST node: (type, entry, left, right)
      create a new node
          -  of type `type'
          -  pointing to a symbol table entry `entry'
          -  pointing to left node `left'
          -  pointing to right node `right'
      increment the number of ast nodes (ast_node_num)
      set the `id' field of the node to the current value of ast_node_num
      the space for creating a new node is allocated from the ast_pool
      return the pointer to the newly created node
*/
ASTNODE* ast_create_node(ASTTYPE type, SYMTBL entry, ASTNODE *left, ASTNODE *right)
{
    ASTNODE *node = (ASTNODE *) mem_get_free(ast_pool, sizeof(ASTNODE));

    node->id = ++ast_node_num;
    node->type = type;
    node->entry = entry;
    node->left = left;
    node->right = right;
    return(node);
}

/* Dump utilities */
/* print out the info from an abstract syntax tree node;
   consult the sample output `YACCexample.out' for
   the print out format expected. */
void ast_print_node(ASTNODE *node)
{
    printf("NODE %d: type %s ", node->id, asttypename[node->type]);
    if (node->left)  printf("leftnode  %d ", node->left->id);
    if (node->right) printf("rightnode %d ", node->right->id);
    printf("\n");
}

/* a recursive preorder traversal procedure (process root, then
   process left subtree, then followed by processing of right child)
   to print out all the nodes in the abstract syntax subtree
   pointed to by `node' */
void ast_dump_subtree(ASTNODE *node)
{
    if (!node) return;
    ast_print_node(node);
    ast_dump_subtree(node->left);
    ast_dump_subtree(node->right);
}

/* print out all nodes in the abstract syntax tree
   make use of the function `ast_dump_subtree'
*/
void ast_dump()
{
    printf("\nABSTRACT SYNTAX TREE DUMP:\n");
    ast_dump_subtree(ast_root);
}
