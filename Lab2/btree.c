#include<stdlib.h>
#include "bTree.h"
#include<stdio.h>
#include <math.h>



struct treecell {
     unsigned int d;
     struct treecell *Left, *Right;
     };

btree NewBTree()
{ 
   return(NULL);
}

btree MakeBTree( btree a, int i, btree c )
{
   btree b=(btree) malloc(sizeof(struct treecell));
   b->d = i;
   b->Left   = a;
   b->Right  = c;
   return(b);
}

boolean IsEmptyBTree( btree a )
{
   return(a==NULL);
}

btree Left( btree a )

{
   return( IsEmptyBTree(a) ? NewBTree() : a->Left ); 
}

btree Right( btree a )
{  
   return( IsEmptyBTree(a) ? NewBTree() : a->Right );
}

int Data( btree a )
{
   return( IsEmptyBTree(a) ? Undefined : a->d );
}


void postorder_traversal( btree tree )
{
	if(tree->Left!=NULL) postorder_traversal(Left(tree));
    if(tree->Right!=NULL) postorder_traversal(Right(tree));
		if((tree->Left==NULL)&& (tree->Right==NULL)){
		printf(" %d ", Data(tree));}
			else{
				if(Data(tree)==2) printf (" + ");
			else if (Data(tree)==3) printf (" - ");
			else if (Data(tree)==4) printf (" * ");
			else if (Data(tree)==5) printf (" / ");
			else if (Data(tree)==7) printf (" ^ ");
}}
/*if (IsEmptyBTree(t))
	{
	
	t = NewBTree();
	
}
else {
	
	// check if there is child, if there is a child
	
	postorder_traversal(Left(t));
	postorder_traversal(Right(t));
	printf("%d ", Data(t));
	
}	
 	
	
}*/

/*
void postorder_traversal( btree t ){

if(!IsEmptyBTree){

Left(t);
Right(t);
printf("%d",Data(t));

}*/
