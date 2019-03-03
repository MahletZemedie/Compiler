
%{
#include "mem.h"
#include "symtbl.h"
#include <stdlib.h>
#include <stdio.h>

int  yylex();
void yyerror(char  *s)  { fprintf(stderr,  "%s\n",  s); exit(-1); }

int data_type; 
int current_scope = 1;

void matchRefDef(ASTNODE *ref, ASTNODE *def) {
   if (ref->left->type == AST_ID &&   ref->left->entry->ast_link  &&  def->left->entry->ast_link && !def->left->left) yyerror("(simultaneous assignment) cannot assign an array to another array");
   if (ref->left->type == AST_ID &&   ref->left->entry->ast_link) yyerror("(simultaneous assignment) attempt to assign an array to a non-array");
   if (def->left->entry->ast_link && !def->left->left) yyerror("(simultaneous assignment) attempt to assign a non array value to an array");
   if (ref->entry->data_type == TYPE_FLOAT && def->left->entry->data_type == TYPE_INT) yyerror("(simultaneous assignment) cannot assign a float value to an integer variable\n");
   if (ref->left->entry->data_type == TYPE_VOID) yyerror("a function call that returns void cannot be on the right hand side of an assignment statement\n");
   ref->type  = AST_ASSIGN;
   ref->right = ref->left;
   ref->left  = ast_create_node( AST_ID,symtbl_new_temp(ref->right->entry->data_type?TYPE_FLOAT:TYPE_INT),0,0);   
   def->right = ref->left;
}

void matchMultAssign(ASTNODE *ref, ASTNODE *def) {
   if (ref->type==AST_EXPR      && def->type==AST_LIST) yyerror("too many ID's on the left side of a simultaneous assignment statement\n");
   if (ref->type==AST_EXPR_LIST && def->type!=AST_LIST) yyerror("too few  ID's on the left side of a simultaneous assignment statement\n");
   if (ref->type==AST_EXPR      && def->type!=AST_LIST) {matchRefDef(ref,def); return;}
   ref->type = AST_LIST;
   matchRefDef(ref->right, def->right);
   matchMultAssign(ref->left, def->left);
}

void matchArgPara(ASTNODE *arg, ASTNODE *par) {
   if (arg->entry->data_type != par->entry->data_type) yyerror("argument type and parameter type mismatch");
   if (arg->left->type == AST_ID && !(arg->left->entry->ast_link) && !par->left) return; 
   if (arg->left->type == AST_ID &&   arg->left->entry->ast_link  &&  par->left) return; 
   if (arg->left->type != AST_ID                                  && !par->left) return;
   yyerror("argument and parameter mismatch; one is an array, the other is not");
}

void matchFuncRefs(ASTNODE *arg, ASTNODE *par) { 
   if (arg->type==AST_EXPR && par->type==AST_PARA_LIST) yyerror("func call has wrong number of arguments"); 
   if (arg->type==AST_EXPR_LIST && par->type==AST_PARA) yyerror("func call has wrong number of arguments"); 
   if (arg->type==AST_EXPR      && par->type==AST_PARA) {matchArgPara(arg,par); return;}
   matchArgPara(arg->right, par->right);
   matchFuncRefs(arg->left, par->left);
}

void matchArrayRefs( ASTNODE *ref, ASTNODE *def ) {
   if (!ref && !def) return;
   if ( ref && !def) yyerror("array ref has different dimension as declared\n"); 
   if (!ref &&  def) yyerror("array ref has different dimension as declared\n");
   ref->entry =  def->entry;
   matchArrayRefs(ref->left, def->left);
}

%}

%union {
        SYMTBL       entry;
        char        *name;
	ASTNODE     *node;
}

%start               programs
%token               VOID BREAK CONTINUE IF ELSE FLOAT INT 
%token               RETURN WHILE OR AND EQ
%token  <name>       ID INTdenotation FLOATdenotation
%type   <node>       program program_l expr_l array_dims para_dims array_indices array_index
%type   <node>       para_decl_l para_decl var_decl_l var_decl 
%type   <node>       compound stmnt_list id_l
%type   <node>       stmnt expr cond disjunction conjunction
%type   <node>       comparison relation sum term factor primary
%type   <entry>      programID array_dim

%%

programs    : program_l
              { ast_root = $1; 
                symtbl_dump_current();
                printf("\n\n");
                ast_dump();
                printf("\n\n"); 
              }
            | var_decl_l program_l
              { ast_root = ast_create_node(AST_LIST, 0, $1, $2); 
                symtbl_dump_current();
                printf("\n\n");
                ast_dump();
                printf("\n\n"); 
              }
            ;

program_l   : program 
              { $$ = $1; }
            | program_l program
              { $$ = ast_create_node(AST_PROGRAM_LIST, 0, $1, $2); }
            ;

program     : type_spec programID '(' para_decl_l ')' {$2->ast_link = $4; current_scope = 2;} compound
              {
                $$ = ast_create_node(AST_PROGRAM, $2, $4, $7);
                $2->local_var_index = local_var_num;                 // *** NEW ***
                ast_dump_subtree($$);
                printf("\n\n");
                symtbl_finalizeScope();
                local_var_num = 0;                                   // *** NEW ***
              }
            | type_spec programID '('             ')' {$2->ast_link = 0;  current_scope = 2;} compound
              {
                $$ = ast_create_node(AST_PROGRAM, $2,  0, $6);
                $2->local_var_index = local_var_num;                 // *** NEW ***
                ast_dump_subtree($$);
                printf("\n\n");
                local_var_num = 0;                                   // *** NEW ***
              }
            | type_void programID '(' para_decl_l ')' {$2->ast_link = $4; current_scope = 2;} compound
              {
                $$ = ast_create_node(AST_PROGRAM, $2, $4, $7);
                $2->local_var_index = local_var_num;                 // *** NEW ***
                ast_dump_subtree($$);
                printf("\n\n");
                symtbl_finalizeScope();
                local_var_num = 0;                                   // *** NEW ***
              }
            | type_void programID '('             ')' {$2->ast_link = 0; current_scope = 2;} compound 
              {
                $$ = ast_create_node(AST_PROGRAM, $2,  0, $6);
                $2->local_var_index = local_var_num;                 // *** NEW ***
                ast_dump_subtree($$);
                printf("\n\n");
                local_var_num = 0;                                   // *** NEW ***
              }
            ;
	
programID   : ID
              { SYMTBL entry = symtbl_lookup($1);
                if ((entry) && (entry->sym_type==SYMBOL_FUNC)) yyerror("function already defined\n");
                $$ = symtbl_install($1,SYMBOL_FUNC,data_type,1); 
              }
            ;

para_decl_l : para_decl
              { $$ = $1; }
            | para_decl_l ',' para_decl
              { $$ = ast_create_node(AST_PARA_LIST, 0, $1, $3); }
            ;

para_decl   : type_spec ID
              { SYMTBL entry = symtbl_lookup($2);
                if ((entry) && (entry->scope==2)) yyerror("parameter already defined\n"); 
                $$ = ast_create_node(AST_PARA, 
                     symtbl_install($2, SYMBOL_PARA, data_type,2), 0, 0); 
              }
            | type_spec ID para_dims
              { SYMTBL entry = symtbl_lookup($2);
                if ((entry) && (entry->scope==2)) yyerror("parameter already defined\n"); 
                entry = symtbl_install($2, SYMBOL_PARA, data_type, 2);
                entry->ast_link = $3;
                $$ = ast_create_node(AST_PARA, entry, $3, 0); 
              }
            ;

para_dims   : '[' ']'
              { $$ = ast_create_node(AST_ARRAY_DIMS, 0,  0, 0); }
            | para_dims array_dim
              { $$ = ast_create_node(AST_ARRAY_DIMS, $2, $1, 0); }
            ;

var_decl_l  : var_decl ';'
              { $$ = $1; }
            | var_decl_l var_decl ';' 
              { $$ = ast_create_node(AST_LIST, 0, $1, $2); }
            ;

var_decl    : type_spec     ID  
              { SYMTBL entry = symtbl_lookup($2);
                if ((entry) && (entry->scope==current_scope)) yyerror("variable name already defined\n");
                symtbl_install($2, SYMBOL_ID, data_type, current_scope);
                $$ = 0;
              }
            | type_spec     ID '=' expr
              { if ($4->entry->data_type == TYPE_VOID) yyerror("a function call that returns void cannot be on the right hand side of an assignment statement\n");
                SYMTBL entry = symtbl_lookup($2);
                if ((entry) && (entry->scope==current_scope)) yyerror("variable name already defined\n");
                entry= symtbl_install($2, SYMBOL_ID, data_type, current_scope);
                if (!entry->ast_link && $4->type == AST_ID && $4->entry->ast_link) yyerror("assignment of an array to a non-array  variable"); 
                if (entry->data_type==TYPE_INT && $4->entry->data_type==TYPE_FLOAT) yyerror("cannot assign a float value to an integer variable\n");
                $$ = ast_create_node(AST_ASSIGN, entry->data_type?Float:Int,
                     ast_create_node(AST_ID, entry, 0, 0), $4);
              }
            | type_spec     ID array_dims
              { SYMTBL entry = symtbl_lookup($2);
                if ((entry) && (entry->scope==current_scope)) yyerror("variable name already defined\n");
                entry = symtbl_install($2, SYMBOL_ID, data_type, current_scope);
                entry->ast_link = $3;
                $$ = ast_create_node(AST_ARRAY_DECL, entry, $3, 0);
              }
            | var_decl  ',' ID array_dims
              { SYMTBL entry = symtbl_lookup($3);
                if ((entry) && (entry->scope==current_scope)) yyerror("variable name already defined\n");
                entry = symtbl_install($3, SYMBOL_ID, data_type, current_scope);
                entry->ast_link = $4;
                $$ = ast_create_node(AST_LIST, 0, $1, 
                     ast_create_node(AST_ARRAY_DECL, entry, $4, 0));
              }
            | var_decl  ',' ID  
              { SYMTBL entry = symtbl_lookup($3);
                if ((entry) && (entry->scope==current_scope)) yyerror("variable name already defined\n");
                symtbl_install($3, SYMBOL_ID, data_type, current_scope);
                $$ = $1;
              }
            | var_decl  ',' ID '=' expr
              { if ($5->entry->data_type == TYPE_VOID) yyerror("a function call that returns void cannot be on the right hand side of an assignment statement\n");
                SYMTBL entry = symtbl_lookup($3);
                if ((entry) && (entry->scope==current_scope)) yyerror("variable name already defined\n");
                entry= symtbl_install($3, SYMBOL_ID, data_type, current_scope);
                if (!entry->ast_link && $5->type == AST_ID && $5->entry->ast_link) yyerror("assignment of an array to a non-array  variable"); 
                if (entry->data_type==TYPE_INT && $5->entry->data_type==TYPE_FLOAT) yyerror("cannot assign a float value to an integer variable\n");
                $$ = ast_create_node(AST_LIST, 0, $1, 
                     ast_create_node(AST_ASSIGN, entry->data_type?Float:Int, 
                     ast_create_node(AST_ID, entry, 0, 0), $5));
              }
            ;

array_dims  : array_dim
              { $$ = ast_create_node(AST_ARRAY_DIMS, $1,  0, 0); }
            | array_dims array_dim
              { $$ = ast_create_node(AST_ARRAY_DIMS, $2, $1, 0); }
            ;

array_dim   : '[' INTdenotation ']'
              { SYMTBL entry = symtbl_lookup($2);
                if (!entry) entry = symtbl_install($2,SYMBOL_NUM,TYPE_INT,0); 
                $$ = entry; 
              } 
            ;

type_spec   : INT 
              { data_type = TYPE_INT; }
            | FLOAT
              { yyerror("float unsupported\n"); }           // *** NEW ***
            ;

type_void   : VOID
              { data_type = TYPE_VOID; }
            ;

compound    : '{' { current_scope++; } var_decl_l stmnt_list '}'
               {
                 $$ = ast_create_node(AST_LIST, 0, $3, $4);
                 symtbl_dump_current();
                 printf("\n\n");
                 symtbl_finalizeScope();
                 current_scope--; 
               }
            | '{' stmnt_list '}'
               {  $$ = $2; }
            | '{'         '}'
               {  $$ = 0;  }
            ;

stmnt_list  : stmnt 
              { $$ = $1; }
            | stmnt_list stmnt
              { $$ = ast_create_node(AST_LIST, 0, $1, $2); }
            ;

stmnt       : compound
              { $$ = $1; }
            | IF '(' expr ')' stmnt ELSE stmnt
              { $$ = ast_create_node(AST_IF_ELSE,   0, $3, 
                     ast_create_node(AST_TWO_SIDED, 0, $5, $7)); }
            | IF '(' expr ')' stmnt 
              { $$ = ast_create_node(AST_IF_THEN,   0, $3, $5) ; }
            | WHILE '(' expr ')' stmnt
              { $$ = ast_create_node(AST_WHILE,     0, $3, $5) ; }
            | BREAK       ';'
              { $$ = ast_create_node(AST_BREAK,     0,  0,  0) ; }
            | CONTINUE    ';'
              { $$ = ast_create_node(AST_CONTINUE,  0,  0,  0) ; }
            | RETURN ';'
              { $$ = ast_create_node(AST_RETURN,    0,  0,  0) ; }
            | RETURN expr ';'
              { $$ = ast_create_node(AST_RETURN,    0, $2,  0) ; }
            | '[' id_l ']' '=' '[' expr_l ']' ';'                         
              {
                matchMultAssign($6, $2);                                
                $$ = ast_create_node(AST_LIST,  0, $6, $2) ;           
              }
            | expr        ';'
              { $$ = $1; }
            |             ';'
              { $$ = 0; }
            ; 

expr        : ID '=' expr 
              { 
                if ($3->entry->data_type == TYPE_VOID) yyerror("a function call that returns void cannot be on the right hand side of an assignment statement\n"); 
                SYMTBL entry = symtbl_lookup($1);
                if (!entry) yyerror("identifier undefined\n");
                if (entry->sym_type==SYMBOL_FUNC) yyerror("identifier name is a function name\n"); 
                if (!entry->ast_link && $3->type == AST_ID && $3->entry->ast_link) yyerror("assignment of an array to a non-array  variable"); 
                if (entry->ast_link) yyerror("assignment of a value to an array\n"); 
                if (entry->data_type==TYPE_INT && $3->entry->data_type==TYPE_FLOAT) yyerror("cannot assign a float value to an integer variable\n"); 
                $$ = ast_create_node(AST_ASSIGN, entry->data_type?Float:Int, 
                     ast_create_node(AST_ID, entry, 0, 0), $3); 
              }
            | ID array_indices '=' expr 
              { 
                if ($4->entry->data_type == TYPE_VOID) yyerror("a function call that returns void cannot be on the right hand side of an assignment statement\n"); 
                SYMTBL entry = symtbl_lookup($1);
                if (!entry) yyerror("identifier undefined\n");
                if (entry->sym_type==SYMBOL_FUNC) yyerror("identifier name is a function name\n");
                if (!entry->ast_link) yyerror("identifier name is not an array\n"); 
                if (!entry->ast_link && $4->type == AST_ID && $4->entry->ast_link) yyerror("assignment of an array to a non-array  variable"); 
                if (entry->data_type==TYPE_INT && $4->entry->data_type==TYPE_FLOAT) yyerror("cannot assign a float value to an integer variable\n"); 
                matchArrayRefs($2,entry->ast_link);
                $$ = ast_create_node(AST_ASSIGN, entry->data_type?Float:Int,
                     ast_create_node(AST_ARRAY_REF, entry, $2, 0), $4); 
              }
            | cond 
              { $$ = $1; }
            ;

cond        : disjunction
              { $$ = $1; }
            | disjunction '?' expr ':' cond
              { 
                if ($1->entry->data_type == TYPE_VOID) yyerror("conditional exp cannot apply to a function that returns void\n"); 
                if ($3->entry->data_type == TYPE_VOID) yyerror("conditional exp cannot apply to a function that returns void\n"); 
                if ($5->entry->data_type == TYPE_VOID) yyerror("conditional exp cannot apply to a function that returns void\n"); 
                if ($1->type == AST_ID && $1->entry->ast_link) yyerror("conditional exp cannot take an array variable as condition"); 
                if ($3->type == AST_ID && $3->entry->ast_link) yyerror("conditional exp cannot return an array variable as result"); 
                if ($5->type == AST_ID && $5->entry->ast_link) yyerror("conditional exp cannot return an array variable as result"); 
                $$ = ast_create_node(AST_COND_EXP, $3->entry->data_type||$5->entry->data_type?Float:Int, $1, 
                     ast_create_node(AST_ALT,      $3->entry->data_type||$5->entry->data_type?Float:Int, $3, $5)); }
            ;

disjunction : conjunction
              { $$ = $1; }
            | disjunction OR conjunction
              { 
                if ($1->entry->data_type == TYPE_VOID) yyerror("|| cannot apply to a function that returns void\n"); 
                if ($3->entry->data_type == TYPE_VOID) yyerror("|| cannot apply to a function that returns void\n"); 
                if ($1->type == AST_ID && $1->entry->ast_link) yyerror("|| cannot take an array variable as operand"); 
                if ($3->type == AST_ID && $3->entry->ast_link) yyerror("|| cannot take an array variable as operand"); 
                if ($1->entry->data_type != TYPE_INT) yyerror("|| can only apply to integer type\n"); 
                if ($3->entry->data_type != TYPE_INT) yyerror("|| can only apply to integer type\n"); 
                $$ = ast_create_node(AST_OR, Int, $1, $3) ; 
              }
            ;

conjunction : comparison
              { $$ = $1; }
            | conjunction AND comparison
              { 
                if ($1->entry->data_type == TYPE_VOID) yyerror("&& cannot apply to a function that returns void\n"); 
                if ($3->entry->data_type == TYPE_VOID) yyerror("&& cannot apply to a function that returns void\n"); 
                if ($1->type == AST_ID && $1->entry->ast_link) yyerror("&& cannot take an array variable as operand"); 
                if ($3->type == AST_ID && $3->entry->ast_link) yyerror("&& cannot take an array variable as operand"); 
                if ($1->entry->data_type != TYPE_INT) yyerror("&& can only apply to integer type\n"); 
                if ($3->entry->data_type != TYPE_INT) yyerror("&& can only apply to integer type\n"); 
                $$ = ast_create_node(AST_AND, Int, $1, $3) ; 
              }
            ;

comparison  : relation 
              { $$ = $1; }
            | relation EQ relation
              { 
                if ($1->entry->data_type == TYPE_VOID) yyerror("relational operator cannot apply to a function that returns void\n"); 
                if ($3->entry->data_type == TYPE_VOID) yyerror("relational operator cannot apply to a function that returns void\n"); 
                if ($1->type == AST_ID && $1->entry->ast_link) yyerror("relational operator cannot take an array variable as operand"); 
                if ($3->type == AST_ID && $3->entry->ast_link) yyerror("relational operator cannot take an array variable as operand"); 
                $$ = ast_create_node(AST_EQ, Int, $1, $3) ; }
            ;

relation    : sum 
              { $$ = $1; }
            | sum '<' sum
              { 
                if ($1->entry->data_type == TYPE_VOID) yyerror("< cannot apply to a function that returns void\n"); 
                if ($3->entry->data_type == TYPE_VOID) yyerror("< cannot apply to a function that returns void\n"); 
                if ($1->type == AST_ID && $1->entry->ast_link) yyerror("< cannot take an array variable as operand"); 
                if ($3->type == AST_ID && $3->entry->ast_link) yyerror("< cannot take an array variable as operand"); 
                $$ = ast_create_node(AST_LT, Int, $1, $3) ; }
            | sum '>' sum
              { 
                if ($1->entry->data_type == TYPE_VOID) yyerror("> cannot apply to a function that returns void\n"); 
                if ($3->entry->data_type == TYPE_VOID) yyerror("> cannot apply to a function that returns void\n"); 
                if ($1->type == AST_ID && $1->entry->ast_link) yyerror("> cannot take an array variable as operand"); 
                if ($3->type == AST_ID && $3->entry->ast_link) yyerror("> cannot take an array variable as operand"); 
                $$ = ast_create_node(AST_GT, Int, $1, $3) ; }
            ;

sum         : term  
              { $$ = $1; }
            | sum '+' term
              { 
                if ($1->entry->data_type == TYPE_VOID) yyerror("+ cannot apply to a function that returns void\n"); 
                if ($3->entry->data_type == TYPE_VOID) yyerror("+ cannot apply to a function that returns void\n"); 
                if ($1->type == AST_ID && $1->entry->ast_link) yyerror("+ cannot take an array variable as operand"); 
                if ($3->type == AST_ID && $3->entry->ast_link) yyerror("+ cannot take an array variable as operand"); 
                $$ = ast_create_node(AST_ADD, $1->entry->data_type||$3->entry->data_type?Float:Int, $1, $3); }
            | sum '-' term
              { 
                if ($1->entry->data_type == TYPE_VOID) yyerror("- cannot apply to a function that returns void\n"); 
                if ($3->entry->data_type == TYPE_VOID) yyerror("- cannot apply to a function that returns void\n"); 
                if ($1->type == AST_ID && $1->entry->ast_link) yyerror("- cannot take an array variable as operand"); 
                if ($3->type == AST_ID && $3->entry->ast_link) yyerror("- cannot take an array variable as operand"); 
                $$ = ast_create_node(AST_SUB, $1->entry->data_type||$3->entry->data_type?Float:Int, $1, $3); }
            ;

term        : factor 
              { $$ = $1; }
            | term '*' factor
              { 
                if ($1->entry->data_type == TYPE_VOID) yyerror("* cannot apply to a function that returns void\n"); 
                if ($3->entry->data_type == TYPE_VOID) yyerror("* cannot apply to a function that returns void\n"); 
                if ($1->type == AST_ID && $1->entry->ast_link) yyerror("* cannot take an array variable as operand"); 
                if ($3->type == AST_ID && $3->entry->ast_link) yyerror("* cannot take an array variable as operand"); 
                $$ = ast_create_node(AST_MUL, $1->entry->data_type||$3->entry->data_type?Float:Int, $1, $3); }
            | term '/' factor
              { 
                if ($1->entry->data_type == TYPE_VOID) yyerror("/ cannot apply to a function that returns void\n"); 
                if ($3->entry->data_type == TYPE_VOID) yyerror("/ cannot apply to a function that returns void\n"); 
                if ($1->type == AST_ID && $1->entry->ast_link) yyerror("/ cannot take an array variable as operand"); 
                if ($3->type == AST_ID && $3->entry->ast_link) yyerror("/ cannot take an array variable as operand"); 
                $$ = ast_create_node(AST_DIV, $1->entry->data_type||$3->entry->data_type?Float:Int, $1, $3); }
            | term '%' factor
              { 
                if ($1->entry->data_type == TYPE_VOID) yyerror("% cannot apply to a function that returns void\n"); 
                if ($3->entry->data_type == TYPE_VOID) yyerror("% cannot apply to a function that returns void\n"); 
                if ($1->type == AST_ID && $1->entry->ast_link) yyerror("% cannot take an array variable as operand"); 
                if ($3->type == AST_ID && $3->entry->ast_link) yyerror("% cannot take an array variable as operand"); 
                if ($1->entry->data_type != TYPE_INT) yyerror("% can only apply to integer type\n"); 
                if ($3->entry->data_type != TYPE_INT) yyerror("% can only apply to integer type\n"); 
                $$ = ast_create_node(AST_MOD, Int, $1, $3) ; }
            ;

factor      : primary 
              { $$ = $1; }
            | '!' factor
              { 
                if ($2->entry->data_type == TYPE_VOID) yyerror("! cannot apply to a function that returns void\n"); 
                if ($2->entry->data_type != TYPE_INT) yyerror("! can only apply to integer type\n"); 
                if ($2->type == AST_ID && $2->entry->ast_link) yyerror("! cannot take an array variable as operand"); 
                $$ = ast_create_node(AST_NOT, Int, $2,  0) ; }
            | '-' factor
              { 
                if ($2->entry->data_type == TYPE_VOID) yyerror("- cannot apply to a function that returns void\n"); 
                if ($2->type == AST_ID && $2->entry->ast_link) yyerror("- cannot take an array variable as operand"); 
                $$ = ast_create_node(AST_UMINUS, $2->entry->data_type?Float:Int, $2,  0) ; }
            ;

primary     : INTdenotation
              { SYMTBL entry = symtbl_lookup($1);
                if (!entry) entry = symtbl_install($1,SYMBOL_NUM,TYPE_INT,0); 
                $$ = ast_create_node(AST_INT,   entry,  0,  0); }

            | FLOATdenotation
              { SYMTBL entry = symtbl_lookup($1);
                if (!entry) entry= symtbl_install($1,SYMBOL_NUM,TYPE_FLOAT,0); 
                $$ = ast_create_node(AST_FLOAT, entry, 0,  0); }

            | ID
              { SYMTBL entry = symtbl_lookup($1);
                if (!entry) yyerror("identifier undefined\n"); 
                if (entry->sym_type==SYMBOL_FUNC) yyerror("function ref without parameters\n"); 
                $$ = ast_create_node(AST_ID, entry, 0,  0); }

            | ID '(' expr_l ')'
              { SYMTBL entry = symtbl_lookup($1);
                if (!entry) yyerror("function undefined\n"); 
                if (entry->sym_type!=SYMBOL_FUNC) yyerror("function ref expected\n"); 
                if (!(entry->ast_link)) yyerror("function takes no parameters; argument provided\n");
                matchFuncRefs($3,entry->ast_link);
                $$ = ast_create_node(AST_FUNC_CALL,  entry, $3,  0); }

            | ID '(' ')'
              { SYMTBL entry = symtbl_lookup($1);
                if (!entry) yyerror("function undefined\n"); 
                if (entry->sym_type!=SYMBOL_FUNC) yyerror("function ref expected\n"); 
                if (entry->ast_link) yyerror("function has parameter, but none provided\n");
                $$ = ast_create_node(AST_FUNC_CALL,  entry, 0,  0); }

            | ID array_indices
              { SYMTBL entry = symtbl_lookup($1);
                if (!entry) yyerror("array identifier undefined\n"); 
                if (entry->sym_type==SYMBOL_FUNC) yyerror("identifier is a function name, not an array name\n");
                if (!entry->ast_link) yyerror("identifier name is not an array\n"); 
                matchArrayRefs($2,entry->ast_link);
                $$ = ast_create_node(AST_ARRAY_REF,  entry, $2,  0); }

            | '(' expr ')'
              { $$ = $2; }
            ;

id_l        : ID
              { SYMTBL entry = symtbl_lookup($1);
                if (!entry) yyerror("identifier undefined\n");
                if (entry->sym_type==SYMBOL_FUNC) yyerror("cannot assign value to a function name");
                $$ = ast_create_node( AST_ASSIGN, 0, ast_create_node(AST_ID, entry, 0,  0), 0);
              } 
            | ID array_indices
              {
                SYMTBL entry = symtbl_lookup($1);
                if (!entry) yyerror("identifier undefined\n");
                if (entry->sym_type==SYMBOL_FUNC) yyerror("identifier name is a function name\n");
                if (!entry->ast_link) yyerror("identifier name is not an array\n");
                matchArrayRefs($2,entry->ast_link);
                $$ = ast_create_node( AST_ASSIGN, 0, ast_create_node(AST_ARRAY_REF, entry, $2, 0), 0);
              }
            | id_l ',' ID
              { SYMTBL entry = symtbl_lookup($3);
                if (!entry) yyerror("identifier undefined\n");
                if (entry->sym_type==SYMBOL_FUNC) yyerror("cannot assign value to a function name");
                $$ = ast_create_node(AST_LIST, 0, $1, ast_create_node(AST_ASSIGN, 0, ast_create_node(AST_ID, entry, 0,  0), 0));
              }
            | id_l ',' ID array_indices
              {
                SYMTBL entry = symtbl_lookup($3);
                if (!entry) yyerror("identifier undefined\n");
                if (entry->sym_type==SYMBOL_FUNC) yyerror("identifier name is a function name\n");
                if (!entry->ast_link) yyerror("identifier name is not an array\n");
                matchArrayRefs($4,entry->ast_link);
                $$ = ast_create_node(AST_LIST, 0, $1, ast_create_node(AST_ASSIGN, 0, ast_create_node(AST_ARRAY_REF, entry, $4, 0), 0));
              }
            ;

expr_l      : expr
              { $$ = ast_create_node(AST_EXPR, $1->entry->data_type?Float:Int, $1, 0); }
            | expr_l ',' expr
              { $$ = ast_create_node(AST_EXPR_LIST, 0, $1,
                     ast_create_node(AST_EXPR, $3->entry->data_type?Float:Int, $3, 0)); }
            ;

array_indices : array_index
                { $$ = ast_create_node(AST_ARRAY_INDEX, 0, 0, $1); }
              | array_indices array_index
                { $$ = ast_create_node(AST_ARRAY_INDEX, 0, $1, $2); }
              ;

array_index : '[' expr ']'
              { if ($2->entry->data_type != TYPE_INT) yyerror("array index not of integer type\n");
                $$ = $2;
              } 
            ;

%%

#include "lex.yy.c"

