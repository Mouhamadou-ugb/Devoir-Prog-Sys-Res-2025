#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>


void Creer_arbre(int niveau_courant, int profondeur) {

    pid_t fils_gauche, fils_droit;

    if (niveau_courant >= profondeur) 
       return ;  

    fils_gauche = fork();
    if (fils_gauche ==-1) { 
        perror("Lors de la creation de processus."); 
        exit(-1);
    } else if (fils_gauche == 0) {                  
        printf("%d -> %d;\n",getppid(), getpid());
        Creer_arbre(niveau_courant + 1, profondeur);
        exit(1);         
    }

    fils_droit = fork();
    if (fils_droit ==-1) { 
        perror("Lors de la creation de processus."); 
        exit(-1);
    } else if (fils_droit == 0) {                  
        printf("%d -> %d;\n",getppid(), getpid());
        Creer_arbre(niveau_courant + 1, profondeur);
        exit(1);
    }

    wait(NULL);
    wait(NULL);
}

// Principe : Le parent de niveau 0 lance la creation avec la profondeur donnee
// Et chaque fils gauche et droite affiche son pid et le pid de sont parent et s' arrete apres
// L'incrementation du niveau courant permet de passer au niveau suivant. Tout en sachant que la profondeur
// donner en parametre ne doit pas etre depasser.

// Le graph exemplaire de l'exercice est dans le fichier graph.pdf

int main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("Usage: %s profondeur.\n", argv[0]);
        return -1;
    }

    int profondeur = atoi(argv[1]);
    if (profondeur < 0) {
        perror("La profondeur doit être >= 0.\n");
        return -1;
    }

    Creer_arbre(0, profondeur);
    return 0;
}