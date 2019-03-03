
/**
 **    Symbol table manager
 **/

#include <stdio.h>
#include <string.h>
#include "symtbl.h"
#include "mem.h"

#define SYMTBL_SIZE    1024
#define STR_TABLE_SIZE 1024
#define SYMTBL_HASH_TABLE_SIZE  32

char *symtbl_type_name[] = 
{
     "ID", "NUM", "FUNC", "PARA", "TEMP"
};

char *data_type_name[] = 
{
    "INT", "FLOAT", "VOID"
};

MEM_POOL symtbl_entry_pool;        /* pool of symbol table entries */
SYMTBL symtbl_hash_table[SYMTBL_HASH_TABLE_SIZE];        /* hash table */
MEM_POOL str_pool;
int   temp_num;     /* number of temp variables, also used to name temps */
int   num_entries;  /* total number of entries used in the symtbl_entry_pool */ 

SYMTBL head_scope_link;

/*  initialize each symbol hash table entry to NULL.

    allocate memory to 'symtbl_entry_pool' and 'str_pool' 
    using 'mem_alloc_pool'. specifically, 'str_pool'
    should hold 'STR_TABLE_SIZE' bytes and
    'symtbl_entry_pool' should hold 'SYMTBL_SIZE' number of 'SYMENTRY's. 

    'temp_num' and 'head_scope_link' are initialized to null.
*/
void symtbl_init()
{int i;
    for (i = 0; i < SYMTBL_HASH_TABLE_SIZE; i++) symtbl_hash_table[i] = 0;
    symtbl_entry_pool = mem_alloc_pool(SYMTBL_SIZE*sizeof(SYMENTRY));
    str_pool = mem_alloc_pool(STR_TABLE_SIZE);
    temp_num = 0;
    head_scope_link = 0;
}

/*  use the following simple hash function:
    add up the ascii value of each character in 'name'
    return `sum' modulo SYMTBL_HASH_TABLE_SIZE
*/
int symtbl_hash(char *name)
{
    /* to be completed */ 
	char *p; int n;

    for (n=0, p=name; *p; p++) n += *p;

    return (n%SYMTBL_HASH_TABLE_SIZE); 
}

/*  return a pointer to a symbol table entry that
    has the name field being 'sym' 

    hint: use 'strcmp' to compare between two strings.

    return NULL if there is no symbol table entry with the name 
    being 'sym'.
*/
SYMTBL symtbl_lookup(char *sym)
{
    /* to be completed */ 
	SYMTBL entry;



    for (entry=symtbl_hash_table[symtbl_hash(sym)];

         entry && strcmp(entry->sym_name,sym);

         entry=entry->next) ;



    return(entry);
	
	
}

/*  install a new symbol table entry with the name and type being
    'sym' and 'stype' where the symbol types are defined as integers
    in symtbl.h

    assume that 'symtbl_lookup' has already been consulted and 
    it is determined that there is not a symbol table entry with
    the name 'sym' in scope level 'scope'

    need to call 'mem_get_free' to allocate memory from the pool
    'symtbl_entry_pool' to hold a new SYMENTRY 

    need to call 'mem_get_free' to allocate memory from the pool
    'str_pool'.' to hold a copy of the string 'sym'

    we use 'strcpy' to copy string 'sym' into the str_pool.
    
    update the linked list appropriately.

    scope for int or float const is 0; scope is non-negative 
    the symbol table entry for a int or float const needs to be 
    appended to the end of the corresponding list in the symtbl_hash_table.

    increase the value of the variable 'num_entries' by 1

    update the scope_link for entries that are of scope > 0
    update the head_scope_link as well.

    return the entry created

*/
SYMTBL symtbl_install(char *sym, int stype, int dtype, int scope)
{
    SYMTBL entry, p; char *name; int index;
    entry = (SYMTBL) mem_get_free(symtbl_entry_pool, sizeof(SYMENTRY));
    name  =          mem_get_free(str_pool, strlen(sym)+1);
    num_entries += 1;
    strcpy(name, sym);
    entry->sym_name = name;
    entry->sym_type = stype;
    entry->data_type= dtype;
    entry->scope    = scope;
    index = symtbl_hash(sym);
    if (scope == 0) {
       entry->next = 0;
       if (!symtbl_hash_table[index]) {
           symtbl_hash_table[index] = entry;
       } else { // append to the end of the list as scope is 0
          for (p=symtbl_hash_table[index];p->next;p=p->next)
              /* do nothing */   ;
          p->next = entry;
       }
    } else {
       entry->next = symtbl_hash_table[index];
       symtbl_hash_table[index] = entry;
       entry->scope_link = head_scope_link;
       head_scope_link = entry;
    }
    return(entry);
}

SYMTBL symtbl_new_temp(int dtype)
{
    char s[10];
    temp_num++;
    sprintf(s, "TEMP%d", temp_num);
    return(symtbl_install(s,SYMBOL_TEMP,dtype,-1));
}

/*  print out the contents of a symbol table entry */
void symtbl_dump_entry(SYMTBL entry)
{
     printf("  Name %s  Scope %d  Symbol Type %s  Data Type %s\n", 
             entry->sym_name, entry->scope,
             symtbl_type_name[entry->sym_type], 
	     data_type_name[entry->data_type]);
}

/*  print out the contents of all current entries found in the symbol table.

    algorithm: 
       search through each bucket of the hash table
       by following the pointers in the lists 
       to print out all the entries.
*/
void symtbl_dump_current()
{
    SYMTBL entry; int i;
    printf("\nSYMBOL TABLE DUMP CURRENT:\n");

    for (i = 0; i < SYMTBL_HASH_TABLE_SIZE; i++)
      for (entry = symtbl_hash_table[i]; entry; entry = entry->next) {
           printf("Hash index %d:\n", i);
           symtbl_dump_entry(entry);
      }
}

/*  print out the contents of all entries (including the obsolete ones) found in the symbol table.

    algorithm: 
       sequentially search through all the entries found in the symtbl_entry_pool.
*/
void symtbl_dump_all()
{
    int i; SYMTBL entry;
    printf("\nSYMBOL TABLE DUMP ALL:\n");
    for (i=0, entry=(SYMTBL)symtbl_entry_pool->pool; i<num_entries; i++, entry++) 
         symtbl_dump_entry(entry);
}

/*   remove the symbols that are defined in the current scope level
     from the symbol table, and from the list pointed to by head_scope_link.
     one can assume that there is at least one symbol defined
     in the current level; otherwise, the current function 
     symtbl_finalizeScope() will not be invoked */
void symtbl_finalizeScope()
{
	 int scope_level = head_scope_link->scope;

    while ((head_scope_link) && (head_scope_link->scope ==  scope_level)) 
	{ 

           int index = symtbl_hash(head_scope_link->sym_name);

           symtbl_hash_table[index] = symtbl_hash_table[index]->next;

           head_scope_link = head_scope_link->scope_link; 

    }
    /* to be completed */
}
