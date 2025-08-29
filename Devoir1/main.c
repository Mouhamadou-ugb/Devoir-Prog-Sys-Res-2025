#include <stdio.h>   
#include <stdlib.h>
#include <unistd.h>  
#include <string.h>

extern int optopt;    
extern char* optarg;  

int Puissance(int x, int n){
   int y = x;
   for (int i = 2; i <= n; i++)
       y = y * x;
   return y;
}

int main(int argc, char *argv[]){
   
   if(argc<3){
      printf("Nombre d'argument insuffisant.\n");
      printf("Usage: %s option(-f entier ou -g entier ou -f entier -g ou -g entier -f)\n",argv[0]);
      return 0;
   }

   int opt, entier = 0, f = 0, g = 0;

   while ((opt = getopt(argc, argv, ":f:g:")) != -1)
   {
       switch (opt) {
            case 'f':    
                         entier = atoi(argv[2]);
                         f=1;
                         break;
            case 'g':
                        
                         entier = atoi(argv[2]) ;
                         g=1;
                         break; 
; 
            case '?':
                       printf("Erreur option -%c inconnue.\n", optopt);
                         break; 
                
            default:
                       entier = atoi(argv[2]);
                       if((strcmp("-f",argv[1]) == 0) && (strcmp("-g",argv[3])==0)){
                         f=-1;
                         g=-2;
                       }else if ((strcmp("-g",argv[1]) == 0) && (strcmp("-f",argv[3])==0)){
                          f=-2;
                          g=-1;
                       }
                       break; 
       }
   }
   if (f==1 && g==0)
      printf("f(%d)= 2^%d = %d\n", entier , entier ,  Puissance(2,entier));
   else if (g==1 && f==0)
      printf("g(%d)= 2 * %d = %d\n", entier , entier ,  2 * entier ) ;
   else if (f==-1 && g==-2 )
      printf("fog(%d)= 2^(2x%d) = %d\n", entier , entier ,  Puissance(2,2*entier) );
   else if (f==-2 && g==-1)
      printf("gof(%d)= 2x2^%d = %d\n", entier , entier ,  2 * Puissance(2,entier) );
   return 0;  
}