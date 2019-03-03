
/**  mem.h 
 ** 
 **     Interface to the memory management module
 **/

typedef struct MEM_POOL_NODE_STRUCT
{
    struct MEM_POOL_NODE_STRUCT  *link;
    char      *next_free;
    int        size;
    char      *pool;
} MEM_POOL_NODE;

typedef MEM_POOL_NODE *MEM_POOL;

/* Init memory management module */
void mem_init();

/* Allocate a new memory pool: 
   MEM_POOL mem_alloc_pool(int size_in_bytes) */ 
MEM_POOL mem_alloc_pool(int size_in_bytes);

/* Reset memory pool: void mem_reset_pool(MEM_POOL pool) 
   This does not deallocate/free the memory pool. 
   It only reset the memory pool so that 
   all the memory in the pool can be re-used. */
void mem_reset_pool(MEM_POOL pool);

/* Deallocate memory pool: void mem_free_pool(MEM_POOL pool) */
void mem_free_pool(MEM_POOL pool);

/* Get free memory space from a memory pool: 
   char *mem_get_free(MEM_POOL pool, int size)  */
char *mem_get_free(MEM_POOL pool, int size);
