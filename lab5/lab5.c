#include <stdio.h>

#define num_states    18
#define alphabet_size 5

int T[num_states][alphabet_size] = {
              1,            19,            19,        19,          19,
             2 ,           19,            19,        19,          19,
             3,             7,               5,        6,          19,
             3 ,            7,               5,         6,           19,
             3 ,           7,               5,         6,           19, 
             3 ,          7,                5,         6,           19,
            3 ,            7,               8,         6,           9,
            3 ,            7,               8 ,        6,           9,
            11,             12,          10,        13,          19,
           11 ,            12,            10,        13,          19,
            11 ,            12,            14,        13,          18,
           11 ,            12,            14,        13,          9,
          11 ,            12,            14,        13,          18,
          11,             12,            14,        13,          18,
        19,             19,            19,        19,          19,
	     19,             19,            19,        19,          19,
		 19,             19,            19,        19,          19,
		  19,             19,            19,        19,          19
};

int letterCode(char c) {
    return (c == 'l') ? 0 : (c == 'b') ? 1 : (c == 'w') ? 2 : (c == 'n') ? 3 : (c == 'a') ? 4 : 5; 
}

int proper(char c) {
    return c=='l' || c=='b' || c=='w' || c=='n' || c=='a';

}

void main() {
   int c, state = 0;

   while ( (c = getchar()) != EOF)  
      if (proper(c))
         state = T[state][letterCode(c)];
      else {
         printf("error: unexpected input symbol \n");
         return;
      }

   printf("%s\n", state==18 ? "accept" : "not accept");
}