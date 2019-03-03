#ifndef ast_h
#define ast_h

#define AST_PROGRAM       0
#define AST_PARA_LIST     1
#define AST_PARA          2
#define AST_LIST          3
#define AST_IF_ELSE       4
#define AST_IF_THEN       5
#define AST_TWO_SIDED     6
#define AST_WHILE         7
#define AST_BREAK         8
#define AST_CONTINUE      9
#define AST_RETURN       10
#define AST_ASSIGN       11
#define AST_ID           12
#define AST_COND_EXP     13
#define AST_ALT          14
#define AST_OR           15
#define AST_AND          16
#define AST_EQ           17
#define AST_LT           18
#define AST_GT           19
#define AST_ADD          20
#define AST_SUB          21
#define AST_MUL          22
#define AST_DIV          23
#define AST_MOD          24
#define AST_NOT          25
#define AST_UMINUS       26
#define AST_INT          27
#define AST_FLOAT        28
#define AST_PROGRAM_LIST 29
#define AST_EXPR_LIST    30
#define AST_EXPR         31
#define AST_FUNC_CALL    32
#define AST_ARRAY_INDEX  33
#define AST_ARRAY_REF    34
#define AST_ARRAY_DECL   35
#define AST_ARRAY_DIMS   36

typedef int ASTTYPE;
typedef struct ASTNODE_DT {
    int                  id;
    ASTTYPE              type;
    struct SYMTBL_ENTRY *entry;
    struct ASTNODE_DT   *left;
    struct ASTNODE_DT   *right;
} ASTNODE;

/*  Initialize the ast module - allocate pool for ast */
void ast_init();

/*  Create AST node: (type, entry, left, right) */
ASTNODE* ast_create_node(ASTTYPE type, struct SYMTBL_ENTRY *entry, ASTNODE *left, ASTNODE *right);

void ast_dump_subtree(ASTNODE *node);
void ast_dump();

extern ASTNODE *ast_root;       /* root node of the syntax tree */

#endif
