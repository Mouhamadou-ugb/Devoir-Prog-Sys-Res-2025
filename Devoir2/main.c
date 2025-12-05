#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define TAILLE_BUFFER 255

extern char *optarg;
extern int optopt;

// Execution
// ./main -c -d 10 -f matrice_binaire.bin -b    ou bien   ./main -c -d 10 -f matrice_binaire.bin
// ./main -c -d 10 -f matrice_texte.txt-t      

// ./main -a -d 10 -f matrice_binaire.bin -b    ou bien   ./main -a -d 10 -f matrice_binaire.bin
// ./main -a -d 10 -f matrice_texte.bin -t

void *Erreur(){
    perror("Erreur : ");
    exit(-1);
}

int** Matrice_aleatoire(int d){
    int **mat = (int **) malloc (d * sizeof(int *));
    if(!mat)
       Erreur();
    for (int i = 0; i < d; i++)
    {
        mat [i] = (int *) malloc(d * sizeof(int));
        if(! mat[i])
           Erreur();
    }

    for (int i = 0; i < d; i++)
       for (int j = 0; j < d; j++)
          mat[i][j] = rand() % 10 + 1; 

    return mat;
}

void Detruire_matrice(int **mat, int dim) {
    for (int i = 0; i < dim; i++) 
        free(mat[i]);
    free(mat);
}

void Ecrire_en_binaire(char *nomfic, int** mat, int dim){
    int fd = open(nomfic, O_WRONLY | O_CREAT | O_TRUNC, 
                           S_IRUSR | S_IWUSR | S_IRGRP | S_IXOTH);
    if(fd == -1)
      Erreur();

    int nbWrite = 0;
    int i;
    
    for ( i = 0; i < dim; i++){
        nbWrite = write(fd, &mat[i], dim * sizeof(int));
        if(nbWrite <= 0)
           Erreur();                   
    }

    close(fd);
    printf("Fichier %s creer avec succes.\n", nomfic);
}

void Ecrire_en_texte(char *nomfic, int **mat, int dim){
    int fd = open(nomfic, O_WRONLY | O_CREAT | O_TRUNC, 
                           S_IRUSR | S_IWUSR | S_IRGRP | S_IXOTH);
    if(fd == -1)
       Erreur();

    int nbWrite = 0;
    int i, j;
    char buffer[TAILLE_BUFFER];

    for ( i = 0; i < dim; i++){
        for (j = 0; j< dim; j++){
		    sprintf(buffer, "%d ", mat[i][j]);
            nbWrite = write(fd, buffer, strlen(buffer));
            if(nbWrite <= 0)
               Erreur();         
        }
        write(fd,"\n", 1);
    }

    close(fd);
    printf("Fichier %s creer avec succes.\n", nomfic);
}

void Afficher_en_binaire(char *nomfic, int dim){
    int fd = open(nomfic, O_RDONLY);

	if (fd == -1)
	   Erreur();

    int i;
    int **mat = Matrice_aleatoire(dim);

	for ( i = 0; i < dim; i++) 
        if (read(fd, mat[i], dim * sizeof(int)) == -1) 
            Erreur();
    
    printf("\nAffichage du contenu du fichier %s en mode binaire :\n\n", nomfic);
    for (int i = 0; i < dim; i++) {
        for (int j = 0; j < dim; j++) 
            printf("%d ", mat[i][j]);
        printf("\n");
    }
    
    Detruire_matrice(mat, dim);
    close(fd);
}

void Afficher_en_texte(char *nomfic, int dim){
    FILE *fichier = fopen(nomfic, "r");
    if (fichier == NULL) 
        Erreur();
    
    int **mat = Matrice_aleatoire(dim);
    
    for (int i = 0; i < dim; i++) {
        for (int j = 0; j < dim; j++) 
            if (fscanf(fichier, "%d", &mat[i][j]) != 1) 
                Erreur();
    }
    
    printf("\nAffichage du contenu du fichier %s en mode texte :\n\n", nomfic);
    for (int i = 0; i < dim; i++) {
        for (int j = 0; j < dim; j++) 
            printf("%d ", mat[i][j]);
        printf("\n");
    }
    
    Detruire_matrice(mat,dim);
    fclose(fichier);
}


int main(int argc, char *argv[])
{
    if (argc < 6){ // par defaut on peut omettre le mode d'affichage binaire qui est le mode par defaut
        printf("Nombre d'argument incorrecte.\n");
        printf("Usage normale : %s -c/-a -d dim -f nom_fichier -t/-b.", argv[0]);
    }

    int opt;
    int c = 0, a = 0, d = 0, t = 0, b = 0, f = 0;
    char *nom_fichier = NULL;

	while( (opt = getopt(argc, argv, ":cabtd:f:") ) != -1){
        switch (opt) {
            case 'c':
                       c = 1;
                       break;

            case 'a':
                       a = 1;
                       break;

            case 'd':
                       d = atoi(optarg);
                       if(d <=0){
                         printf("Erreur la dim doit etre > 0.\n");
                         return 0;
                       }
                       break;

            case 'f':
                       f = 1;
                       nom_fichier = optarg;
                       break;

            case 't':
                       t = 1;
                       break;

            case 'b':
                       b = 1;
                       break;

            case ':':
                       printf("Erreur : L'option -%c  attend un argument \n",optopt );
                       break;

            case '?':
                       printf("Erreur : Option %c inconnue\n", optopt);
                       break; 
        }
    }


    if (nom_fichier == NULL) {
        printf("Le nom du fichier est obligatoire.\n");
        return 0;
    }
    
    if (!c && !a) {
        printf("Il faut afficher ou creer un fichier.\n");
        return 0;
    }
    
    if (c && a) {
        printf("Impossible de créer et afficher en même temps\n");
        return 0;
    }
   
    if (c && d && f){   
        int **mat = Matrice_aleatoire(d);
        if(t == 1)
            Ecrire_en_texte(nom_fichier, mat, d);

        else // par defaut le mode est binaire
            Ecrire_en_binaire(nom_fichier, mat, d);   

        Detruire_matrice(mat,d);       
    }

    else if (a && d && f){
        if(t)
          Afficher_en_texte(nom_fichier, d);
        else
          Afficher_en_binaire(nom_fichier, d);
    }

    return 0;
}

