public class example {

public static 
int GCD(int x, int y)
{
   while (!(x == y))
      if (x > y) x = x-y;
      else y = y-x;
   return x;
}

public static 
int Factorial(int v)
{
   int limit = 7;

   if (v < 0 || v > limit) return -1;
   {
       int c = 0, fact = 1;

       /* Loop invariant: fact == c! */
       while (c < v) {
          c = c+1; fact = fact*c;
       }
       return fact;
   }
}

}