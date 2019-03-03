
#include <stdio.h>
#include <string.h>
#include "symtbl.h"
#include "mem.h"

int main(){

   SYMTBL entry;

   mem_init(); 
   symtbl_init();
   
  

   symtbl_install("fact", SYMBOL_ID,  TYPE_INT,   1);
   symtbl_install("v",    SYMBOL_ID,  TYPE_FLOAT, 2);
   symtbl_install("i",    SYMBOL_ID,  TYPE_INT,   2);
   symtbl_install("ii",   SYMBOL_ID,  TYPE_INT,  2);
   symtbl_install("i",    SYMBOL_ID,  TYPE_FLOAT, 3);
   symtbl_install("r",    SYMBOL_ID,  TYPE_FLOAT, 3);
   symtbl_install("2",    SYMBOL_NUM, TYPE_INT,   0);
   
   //test case 
   symtbl_install("m",    SYMBOL_ID,  TYPE_INT,  2);
   symtbl_install("n",    SYMBOL_ID,  TYPE_FLOAT, 1);
   symtbl_install("x",    SYMBOL_ID,  TYPE_FLOAT, 3);
   symtbl_install("y",    SYMBOL_NUM, TYPE_INT,   0);
   
   
   
   
 
   entry = symtbl_lookup("i");
   if (entry) 
        printf("variable i is found and has scope %d and data type %s \n", 
                entry->scope, entry->data_type == 0 ? "integer" : "float" );
   else       
        printf("variable i is not found\n");
   symtbl_dump_current();
   printf("============================\n");
   symtbl_dump_all();
   printf("\n");
   printf("============================\n");
   symtbl_finalizeScope();
   entry = symtbl_lookup("i");
   if (entry) 
        printf("variable i is found and has scope %d and data type %s \n", 
                entry->scope, entry->data_type == 0 ? "integer" : "float" );
   else       
        printf("variable i is not found\n");
   symtbl_dump_current();
   printf("============================\n");
   symtbl_dump_all();
   printf("============================\n");
   symtbl_finalizeScope();
   symtbl_dump_current();
   printf("============================\n");
   symtbl_dump_all();
   
   
}

/*********** Outputs are: *************

variable i is found and has scope 3 and data type float 

SYMBOL TABLE DUMP CURRENT:
Hash index 9:
  Name i  Scope 3  Symbol Type ID  Data Type FLOAT
Hash index 9:
  Name i  Scope 2  Symbol Type ID  Data Type INT
Hash index 18:
  Name r  Scope 3  Symbol Type ID  Data Type FLOAT
Hash index 18:
  Name ii  Scope 2  Symbol Type ID  Data Type INT
Hash index 18:
  Name 2  Scope 0  Symbol Type NUM  Data Type INT
Hash index 22:
  Name v  Scope 2  Symbol Type ID  Data Type FLOAT
Hash index 30:
  Name fact  Scope 1  Symbol Type ID  Data Type INT
============================

SYMBOL TABLE DUMP ALL:
  Name fact  Scope 1  Symbol Type ID  Data Type INT
  Name v  Scope 2  Symbol Type ID  Data Type FLOAT
  Name i  Scope 2  Symbol Type ID  Data Type INT
  Name ii  Scope 2  Symbol Type ID  Data Type INT
  Name i  Scope 3  Symbol Type ID  Data Type FLOAT
  Name r  Scope 3  Symbol Type ID  Data Type FLOAT
  Name 2  Scope 0  Symbol Type NUM  Data Type INT

============================
variable i is found and has scope 2 and data type integer 

SYMBOL TABLE DUMP CURRENT:
Hash index 9:
  Name i  Scope 2  Symbol Type ID  Data Type INT
Hash index 18:
  Name ii  Scope 2  Symbol Type ID  Data Type INT
Hash index 18:
  Name 2  Scope 0  Symbol Type NUM  Data Type INT
Hash index 22:
  Name v  Scope 2  Symbol Type ID  Data Type FLOAT
Hash index 30:
  Name fact  Scope 1  Symbol Type ID  Data Type INT
============================

SYMBOL TABLE DUMP ALL:
  Name fact  Scope 1  Symbol Type ID  Data Type INT
  Name v  Scope 2  Symbol Type ID  Data Type FLOAT
  Name i  Scope 2  Symbol Type ID  Data Type INT
  Name ii  Scope 2  Symbol Type ID  Data Type INT
  Name i  Scope 3  Symbol Type ID  Data Type FLOAT
  Name r  Scope 3  Symbol Type ID  Data Type FLOAT
  Name 2  Scope 0  Symbol Type NUM  Data Type INT
============================

SYMBOL TABLE DUMP CURRENT:
Hash index 18:
  Name 2  Scope 0  Symbol Type NUM  Data Type INT
Hash index 30:
  Name fact  Scope 1  Symbol Type ID  Data Type INT
============================

SYMBOL TABLE DUMP ALL:
  Name fact  Scope 1  Symbol Type ID  Data Type INT
  Name v  Scope 2  Symbol Type ID  Data Type FLOAT
  Name i  Scope 2  Symbol Type ID  Data Type INT
  Name ii  Scope 2  Symbol Type ID  Data Type INT
  Name i  Scope 3  Symbol Type ID  Data Type FLOAT
  Name r  Scope 3  Symbol Type ID  Data Type FLOAT
  Name 2  Scope 0  Symbol Type NUM  Data Type INT

*/