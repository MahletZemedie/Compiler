
#ifndef bTree
#define bTree

#define Undefined -1
#define boolean int

typedef struct treecell *btree;

btree NewBTree(void);

btree MakeBTree( btree, int, btree );

boolean IsEmptyBTree( btree );

btree Left( btree );

btree Right( btree );

int Data( btree a );
void postorder_traversal( btree t );

#endif
