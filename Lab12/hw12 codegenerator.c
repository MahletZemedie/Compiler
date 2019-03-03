#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtbl.h"
#include "mem.h"
#define ERROR(msg)  { fprintf(stderr, "%s\n", msg); exit(-1); }

int yyparse();

FILE *fp;                // global variable
char *classname;         // global variable
int label = 1;
int return_type;


void print_header(char *classname) {
   fprintf(fp, ".class public %s\n", classname);
   fprintf(fp, ".super java/lang/Object\n\n");
   fprintf(fp, ".method public ()V\n");
   fprintf(fp, "   .limit stack 1\n");
   fprintf(fp, "   aload_0\n");
   fprintf(fp, "   invokespecial java/lang/Object/()V\n");
   fprintf(fp, "   return\n");
   fprintf(fp, ".end method\n\n\n");
}

main(int argc, char *argv[])
{
   char *filename, *output_filename;
   filename = *++argv;
   if (strlen(filename) < 2 || filename[strlen(filename)-1] != 'c' ||
                               filename[strlen(filename)-2] != '.' )
      ERROR("file name does not have extension .c");
   classname = malloc(23);
   strncpy(classname, filename, strlen(filename)-2);
   classname[strlen(filename)-2] = '\0';
   output_filename = malloc(23);
   sprintf(output_filename, "%s.j", classname);
   if ((stdin = fopen(filename,        "r")) == NULL) ERROR("Cannot open input file\n");
   if ((fp    = fopen(output_filename, "w")) == NULL) ERROR("Cannot open output file\n");

   mem_init();
   symtbl_init();
   ast_init();
   yyparse();
   ast_dump();
   print_header(classname);
   traverse_ast(ast_root,0,0,0,0,0,0);
   fclose(fp);
   fclose(stdin);
}



void printSym( SYMTBL symbol ) {
    printf("%s", symbol->sym_name);
    if (symbol->scope > 1) printf("_%d", symbol->scope);
}

int boolExpr( ASTNODE *node ) { 
   if (node->type==AST_ASSIGN) return boolExpr(node->right);
   return (node->type==AST_AND) || (node->type==AST_OR) || (node->type==AST_NOT) 
       || (node->type==AST_LT ) || (node->type==AST_GT) || (node->type==AST_EQ);
}

void coerce_int_to_bool(ASTNODE *node) { 
   SYMTBL temp,temp_0;
   if (boolExpr(node)) return;
   temp_0 = symtbl_new_temp(TYPE_INT);
   printf("     ");
   printSym(temp_0); printf(" = 0\n");
   if (node->entry->sym_type == SYMBOL_ID) {   
       temp   = symtbl_new_temp(TYPE_INT);
       printf("     ");
       printSym(temp);printf(" = ");printSym(node->entry); printf("\n");
       node->entry = temp;
   }
   printf("     ");
   printSym(node->entry);printf(" = ");
   printSym(node->entry);printf(" != "); printSym(temp_0);
   printf("\n");
}

/*  Traverse the syntax tree and build flow graph */
void traverse_ast(ASTNODE *node, int break_Label, int cont_Label) {
    SYMTBL temp;
    int    targetLabel, targetLabel1;

    switch(node->type) 
    {
        case AST_PROGRAM_LIST:
	  traverse_ast(node->left,break_Label,cont_Label);
	  traverse_ast(node->right,break_Label,cont_Label);
	  break;
        case AST_PROGRAM:
          printf("\n\n"); 
          printf("     ");
          printf("FUNC ENTRY ");printSym(node->entry);printf("\n");
          return_type = node->entry->data_type;
          if (node->right)
	     traverse_ast(node->right,break_Label,cont_Label);
          printf("     ");
          printf("FUNC END ");printSym(node->entry);printf("\n");
	  break;
	case AST_LIST:
	  if (node->left)  traverse_ast(node->left,break_Label,cont_Label);
	  if (node->right) traverse_ast(node->right,break_Label,cont_Label);
	  break;
        case AST_FUNC_CALL:
          if (node->left) traverse_ast(node->left,break_Label,cont_Label);
          temp = (node->entry->data_type==TYPE_VOID) ? 0 : symtbl_new_temp(node->entry->data_type);
          printf("     ");
          if (temp) {printSym(temp);printf(" = ");}
          printf("CALL ");printSym(node->entry);printf("\n");
          node->entry = temp;
	  break;
        case AST_EXPR_LIST:
          traverse_ast(node->left,break_Label,cont_Label);
          traverse_ast(node->right,break_Label,cont_Label);
	  break;
        case AST_EXPR:
          traverse_ast(node->left,break_Label,cont_Label);
          printf("     ");
          printf("ARG ");printSym(node->left->entry);printf("\n");
	  break;
	case AST_ASSIGN:
          traverse_ast(node->right,break_Label,cont_Label);
          temp = node->right->entry;
          if (node->left->entry->data_type == TYPE_INT && 
              node->right->entry->data_type == TYPE_FLOAT)
              ERROR("assignment of a float value to an integer variable is not allowed");
          if (node->left->entry->data_type == TYPE_FLOAT && 
              node->right->entry->data_type == TYPE_INT)
             {
              temp = symtbl_new_temp(TYPE_FLOAT);
              printf("     ");
              printSym(temp);printf(" = ");printSym(node->right->entry);printf("\n");
             }
          if (node->left->left) {
              traverse_ast(node->left->left,break_Label,cont_Label);
              printf("     ");
              printSym(node->left->entry);printf("[");
              printSym(node->left->left->entry);printf("] = ");
              printSym(temp);printf("\n");
          } else {
              printf("     ");
              printSym(node->left->entry);printf(" = ");printSym(temp);printf("\n");
          }
          node->entry = temp;
	  break;
        case AST_ARRAY_REF:
          traverse_ast(node->left,break_Label,cont_Label);
          temp = symtbl_new_temp(node->entry->data_type);
          printf("     ");
          printSym(temp);printf(" = ");
          printSym(node->entry);printf("[");
          printSym(node->left->entry);printf("]\n");
          node->entry = temp;
	  break;
        case AST_ARRAY_INDEX:
          traverse_ast(node->right,break_Label,cont_Label);
          if (!node->left) {node->entry = node->right->entry; break;}
          traverse_ast(node->left,break_Label,cont_Label);
          temp = symtbl_new_temp(TYPE_INT);
          printf("     ");
          printSym(temp);printf(" = ");printSym(node->entry);printf("\n");
          node->entry = temp;
          temp = symtbl_new_temp(TYPE_INT);
          printf("     ");
          printSym(temp);printf(" = ");
          printSym(node->left->entry);printf(" * ");
          printSym(node->entry);printf("\n");
          node->entry = symtbl_new_temp(TYPE_INT);
          printf("     ");
          printSym(node->entry);printf(" = ");
          printSym(temp);printf(" + ");
          printSym(node->right->entry);printf("\n");
	  break;
        case AST_RETURN:
          if (return_type == TYPE_VOID &&  node->left) ERROR("function type is void but a data is returned");
          if (return_type == TYPE_VOID) { // !node->left 
             printf("     ");
             printf("RETURN\n");
             break;
          }
          if (return_type == TYPE_INT   && !node->left) ERROR("cannot RETURN void; expected to return an integer");
          if (return_type == TYPE_FLOAT && !node->left) ERROR("cannot RETURN void; expected to return a float");
          traverse_ast(node->left,break_Label,cont_Label);
          if (return_type == TYPE_INT && 
              node->left->entry->data_type == TYPE_FLOAT)
              ERROR("function type is int but a float data is returned");
          temp = node->left->entry;
          if (return_type == TYPE_FLOAT &&
              node->left->entry->data_type == TYPE_INT)
             {
              temp = symtbl_new_temp(TYPE_FLOAT);
              printf("     ");
              printSym(temp);printf(" = ");printSym(node->left->entry);printf("\n");
             }
          printf("     ");
          printf("RETURN ");printSym(temp);printf("\n");
          break;
	case AST_ID:
          if (node->entry->sym_type==SYMBOL_TEMP) break;                  // NEW for HW10
          temp = symtbl_new_temp(node->entry->data_type);
          printf("     ");
          printSym(temp);printf(" = ");printSym(node->entry);printf("\n");
          node->entry = temp;
	  break;
	case AST_INT:
          temp = symtbl_new_temp(TYPE_INT);
          printf("     ");
          printSym(temp);printf(" = ");printSym(node->entry);printf("\n");
          node->entry = temp;
	  break;
	case AST_FLOAT:
          temp = symtbl_new_temp(TYPE_FLOAT);
          printf("     ");
          printSym(temp);printf(" = ");printSym(node->entry);printf("\n");
          node->entry = temp;
	  break;
	case AST_ADD:
          traverse_ast(node->left,break_Label,cont_Label);
	  traverse_ast(node->right,break_Label,cont_Label);
	  node->entry = symtbl_new_temp(
                           node->left->entry->data_type ==
                           node->right->entry->data_type ? 
                           node->right->entry->data_type : TYPE_FLOAT );
          if (node->left->entry->data_type == TYPE_INT &&
              node->right->entry->data_type == TYPE_FLOAT)
             {
              temp = symtbl_new_temp(TYPE_FLOAT);
              printf("     ");
              printSym(temp);printf(" = ");printSym(node->left->entry);printf("\n");
              node->left->entry = temp;
             } 
          if (node->left->entry->data_type == TYPE_FLOAT &&
              node->right->entry->data_type == TYPE_INT)
             {
              temp = symtbl_new_temp(TYPE_FLOAT);
              printf("     ");
              printSym(temp);printf(" = ");printSym(node->right->entry);printf("\n");
              node->right->entry = temp;
             } 
          printf("     ");
          printSym(node->entry);printf(" = ");
          printSym(node->left->entry);printf(" + ");
          printSym(node->right->entry);printf("\n");
	  break;
	case AST_SUB:
          traverse_ast(node->left,break_Label,cont_Label);
          traverse_ast(node->right,break_Label,cont_Label);
          node->entry = symtbl_new_temp(
                           node->left->entry->data_type ==
                           node->right->entry->data_type ? 
                           node->right->entry->data_type : TYPE_FLOAT );
          if (node->left->entry->data_type == TYPE_INT &&
              node->right->entry->data_type == TYPE_FLOAT)
             {
              temp = symtbl_new_temp(TYPE_FLOAT);
              printf("     ");
              printSym(temp);printf(" = ");printSym(node->left->entry);printf("\n"); 
              node->left->entry = temp;
             }    
          if (node->left->entry->data_type == TYPE_FLOAT &&
              node->right->entry->data_type == TYPE_INT)
             {
              temp = symtbl_new_temp(TYPE_FLOAT);
              printf("     ");
              printSym(temp);printf(" = ");printSym(node->right->entry);printf("\n");
              node->right->entry = temp;
             }
          printf("     ");
          printSym(node->entry);printf(" = ");
          printSym(node->left->entry);printf(" - ");
          printSym(node->right->entry);printf("\n"); 
          break;
	case AST_MUL:
          traverse_ast(node->left,break_Label,cont_Label);
          traverse_ast(node->right,break_Label,cont_Label);
          node->entry = symtbl_new_temp(
                           node->left->entry->data_type ==
                           node->right->entry->data_type ? 
                           node->right->entry->data_type : TYPE_FLOAT );
          if (node->left->entry->data_type == TYPE_INT &&
              node->right->entry->data_type == TYPE_FLOAT)
             {
              temp = symtbl_new_temp(TYPE_FLOAT);
              printf("     ");
              printSym(temp);printf(" = ");printSym(node->left->entry);printf("\n");
              node->left->entry = temp;
             }    
          if (node->left->entry->data_type == TYPE_FLOAT &&
              node->right->entry->data_type == TYPE_INT)
             {
              temp = symtbl_new_temp(TYPE_FLOAT);
              printf("     ");
              printSym(temp);printf(" = ");printSym(node->right->entry);printf("\n");
              node->right->entry = temp;
             }
          printf("     ");
          printSym(node->entry);printf(" = ");
          printSym(node->left->entry);printf(" * ");
          printSym(node->right->entry);printf("\n"); 
          break;
	case AST_DIV:
          traverse_ast(node->left,break_Label,cont_Label);
          traverse_ast(node->right,break_Label,cont_Label);
          node->entry = symtbl_new_temp(
                           node->left->entry->data_type ==
                           node->right->entry->data_type ? 
                           node->right->entry->data_type : TYPE_FLOAT );
          if (node->left->entry->data_type == TYPE_INT &&
              node->right->entry->data_type == TYPE_FLOAT)
             {
              temp = symtbl_new_temp(TYPE_FLOAT);
              printf("     ");
              printSym(temp);printf(" = ");printSym(node->left->entry);printf("\n");
              node->left->entry = temp;
             }    
          if (node->left->entry->data_type == TYPE_FLOAT &&
              node->right->entry->data_type == TYPE_INT)
             {
              temp = symtbl_new_temp(TYPE_FLOAT);
              printf("     ");
              printSym(temp);printf(" = ");printSym(node->right->entry);printf("\n");
              node->right->entry = temp;
             }
          printf("     ");
          printSym(node->entry);printf(" = ");
          printSym(node->left->entry);printf(" / ");
          printSym(node->right->entry);printf("\n");
          break;
	case AST_MOD:
          traverse_ast(node->left,break_Label,cont_Label);
          if (node->left->entry->data_type != TYPE_INT)
             ERROR("left operand of modulus is not integer type");
	  traverse_ast(node->right,break_Label,cont_Label);
          if (node->right->entry->data_type != TYPE_INT)
             ERROR("right operand of modulus is not integer type");
	  temp = symtbl_new_temp(TYPE_INT);
          printf("     ");
          printSym(temp);printf(" = ");
          printSym(node->left->entry);printf(" %c ", 37);
          printSym(node->right->entry);printf("\n"); 
	  node->entry = temp;  
	  break;
	case AST_UMINUS:
          traverse_ast(node->left,break_Label,cont_Label);
	  temp = symtbl_new_temp(node->left->entry->data_type);
          printf("     ");
          printSym(temp);printf(" = - ");
          printSym(node->left->entry);printf("\n"); 
	  node->entry = temp;
	  break;
	case AST_AND:
          traverse_ast(node->left,break_Label,cont_Label);
          if (node->left->entry->data_type != TYPE_INT)
             ERROR("left operand of && is not integer type");
          coerce_int_to_bool(node->left); 
	  node->entry = node->left->entry;
          printf("     ");
          printf("if !");printSym(node->entry);printf(" goto %d\n",label);
          targetLabel = label++;
	  traverse_ast(node->right,break_Label,cont_Label);
          if (node->right->entry->data_type != TYPE_INT)
             ERROR("right operand of && is not integer type");
          coerce_int_to_bool(node->right); 
          printf("     ");
          printSym(node->entry);printf(" = ");printSym(node->right->entry);printf("\n");
          printf("%d:\n", targetLabel);
	  break;
	case AST_OR:
          traverse_ast(node->left,break_Label,cont_Label);
          if (node->left->entry->data_type != TYPE_INT)
             ERROR("left operand of || is not integer type");
          coerce_int_to_bool(node->left); 
	  node->entry = node->left->entry;
          printf("     ");
          printf("if ");printSym(node->entry);printf(" goto %d\n",label);
          targetLabel = label++;
          traverse_ast(node->right,break_Label,cont_Label);
          if (node->right->entry->data_type != TYPE_INT)
             ERROR("right operand of || is not integer type");
          coerce_int_to_bool(node->right); 
          printf("     ");
          printSym(node->entry);printf(" = ");printSym(node->right->entry);printf("\n");
          printf("%d:\n", targetLabel);
          break;
	case AST_NOT:
          traverse_ast(node->left,break_Label,cont_Label);
          if (node->left->entry->data_type != TYPE_INT)
             ERROR("operand of ! is not integer type");
	  node->entry = symtbl_new_temp(TYPE_INT);
          if (boolExpr(node->left)) {
             printf("     ");
             printSym(node->entry);printf(" = ! ");
             printSym(node->left->entry);printf("\n");
          }
          else {
             temp = symtbl_new_temp(TYPE_INT);
             printf("     ");
             printSym(temp);printf(" = 0\n");
             printf("     ");
             printSym(node->entry);printf(" = ");
             printSym(node->left->entry);printf(" == ");printSym(temp);printf("\n");
          }
	  break;
	case AST_LT:
          traverse_ast(node->left,break_Label,cont_Label);
          traverse_ast(node->right,break_Label,cont_Label);
          node->entry = symtbl_new_temp(TYPE_INT);
          if (node->left->entry->data_type == TYPE_INT &&
              node->right->entry->data_type == TYPE_FLOAT)
             {
              temp = symtbl_new_temp(TYPE_FLOAT);
              printf("     ");
              printSym(temp);printf(" = ");printSym(node->left->entry);printf("\n");
              node->left->entry = temp;
             }
          if (node->left->entry->data_type == TYPE_FLOAT &&
              node->right->entry->data_type == TYPE_INT)
             {
              temp = symtbl_new_temp(TYPE_FLOAT);
              printf("     ");
              printSym(temp);printf(" = ");printSym(node->right->entry);printf("\n");
              node->right->entry = temp;
             }
          printf("     ");
          printSym(node->entry);printf(" = ");
          printSym(node->left->entry);printf(" < ");
          printSym(node->right->entry);printf("\n"); 
          break;
	case AST_GT:
          traverse_ast(node->left,break_Label,cont_Label);
          traverse_ast(node->right,break_Label,cont_Label);
          node->entry = symtbl_new_temp(TYPE_INT);
          if (node->left->entry->data_type == TYPE_INT &&
              node->right->entry->data_type == TYPE_FLOAT)
             {
              temp = symtbl_new_temp(TYPE_FLOAT);
              printf("     ");
              printSym(temp);printf(" = ");printSym(node->left->entry);printf("\n");
              node->left->entry = temp;
             }
          if (node->left->entry->data_type == TYPE_FLOAT &&
              node->right->entry->data_type == TYPE_INT)
             {
              temp = symtbl_new_temp(TYPE_FLOAT);
              printf("     ");
              printSym(temp);printf(" = ");printSym(node->right->entry);printf("\n");
              node->right->entry = temp;
             }
          printf("     ");
          printSym(node->entry);printf(" = ");
          printSym(node->left->entry);printf(" > ");
          printSym(node->right->entry);printf("\n"); 
          break;
	case AST_EQ:
          traverse_ast(node->left,break_Label,cont_Label);
          traverse_ast(node->right,break_Label,cont_Label);
          node->entry = symtbl_new_temp(TYPE_INT);
          if (node->left->entry->data_type == TYPE_INT &&
              node->right->entry->data_type == TYPE_FLOAT)
             {
              temp = symtbl_new_temp(TYPE_FLOAT);
              printf("     ");
              printSym(temp);printf(" = ");printSym(node->left->entry);printf("\n");
              node->left->entry = temp;
             }
          if (node->left->entry->data_type == TYPE_FLOAT &&
              node->right->entry->data_type == TYPE_INT)
             {
              temp = symtbl_new_temp(TYPE_FLOAT);
              printf("     ");
              printSym(temp);printf(" = ");printSym(node->right->entry);printf("\n");
              node->right->entry = temp;
             }
          printf("     ");
          printSym(node->entry);printf(" = ");
          printSym(node->left->entry);printf(" == ");
          printSym(node->right->entry);printf("\n");
          break;
	case AST_IF_THEN:
	  traverse_ast(node->left,break_Label,cont_Label);
          if (node->left->entry->data_type != TYPE_INT)
             ERROR("INT type expected for the expression");
          printf("     ");
          printf("if !");printSym(node->left->entry);printf(" goto %d\n",label);
          targetLabel = label++;
	  traverse_ast(node->right,break_Label,cont_Label);
          printf("%d:\n", targetLabel);
	  break;
        case AST_IF_ELSE:
          traverse_ast(node->left,break_Label,cont_Label);
          if (node->left->entry->data_type != TYPE_INT)
             ERROR("INT type expected for the expression");
          printf("     ");
          printf("if !");printSym(node->left->entry);printf(" goto %d\n",label);
          targetLabel = label++;
          traverse_ast(node->right->left,break_Label,cont_Label);
          printf("     ");
          printf("goto %d\n",label);
          printf("%d:\n", targetLabel);
          targetLabel= label++;
          traverse_ast(node->right->right,break_Label,cont_Label);
          printf("%d:\n", targetLabel);
          break;
        case AST_COND_EXP:
          traverse_ast(node->left,break_Label,cont_Label);
          if (node->left->entry->data_type != TYPE_INT)
             ERROR("INT type expected for the expression");
          targetLabel = label++;
          printf("     ");
          printf("if ");printSym(node->left->entry);printf(" goto %d\n",targetLabel);
          traverse_ast(node->right->right,break_Label,cont_Label);
          targetLabel1 = label++;
          printf("     ");
          printf("goto %d\n",targetLabel1);
          printf("%d:\n", targetLabel);
          traverse_ast(node->right->left,break_Label,cont_Label);
          temp = symtbl_new_temp( 
                   node->right->left->entry->data_type ==
                   node->right->right->entry->data_type ?
                   node->right->left->entry->data_type : TYPE_FLOAT);
          node->entry = temp;
          printf("     ");
          printSym(temp);printf(" = ");printSym(node->right->left->entry);printf("\n");
          printf("     ");
          printf("goto %d\n",label); 
          printf("%d:\n", targetLabel1);
          printf("     ");
          printSym(temp);printf(" = ");printSym(node->right->right->entry);printf("\n");
          printf("%d:\n", label);
          label++;
          break;
        case AST_WHILE:
          targetLabel = label;
          printf("%d:\n", targetLabel);
          label = label+2;
          traverse_ast(node->left,break_Label,cont_Label);
          if (node->left->entry->data_type != TYPE_INT)
             ERROR("INT type expected for the expression");
          printf("     ");
          printf("if !");printSym(node->left->entry);printf(" goto %d\n", targetLabel+1);
          traverse_ast(node->right,targetLabel+1,targetLabel);
          printf("     ");
          printf("goto %d\n",targetLabel); 
          printf("%d:\n", targetLabel+1);
          break;
        case AST_BREAK:
          if (!break_Label) ERROR("break: no matching while statement");
          printf("     ");
          printf("goto %d\n",break_Label); 
          break; 
        case AST_CONTINUE:
          if (!cont_Label) ERROR("continue: no matching while statement");
          printf("     ");
          printf("goto %d\n",cont_Label); 
          break;  
        case AST_ARRAY_DECL:
        case AST_ARRAY_DIMS:
          break;
	default:
	  ERROR("Unknown AST node");
     }
}
