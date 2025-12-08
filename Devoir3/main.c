#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define TAILLE_MAX_NOM 256
#define TAILLE_BUFFER 4096

// Structure des donnees d'un fichier
typedef struct {
    char nom[TAILLE_MAX_NOM]; 
    struct stat st;  // taille et permissions comprises
} FICHIER;

void Creer_archive(char *nom_archive, char **nFichiers, int nb_fichiers) {

    int fd_archive = open(nom_archive, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_archive == -1) {
        perror("Erreur lors de la creation de l'archive");
        exit(-1);
    }

    // J'ecris le nombre total de fichiers dans l'archive 
    int nbWrite = write(fd_archive, &nb_fichiers, sizeof(int));
    if (nbWrite <= 0) {
        perror("Erreur d'ecriture du nombre de fichiers");
        close(fd_archive);
        exit(-1);
    }

    // Je reserve de la place pour n fichiers
    FICHIER *infos_fichiers = malloc(nb_fichiers * sizeof(FICHIER));  // Ici on a un tableau de n fichiers
    if (infos_fichiers == NULL) {
        perror("Pas assez de memoire");
        close(fd_archive);
        exit(-1);
    }

    // Je parcours chaque fichier et recupere ses informations et l' enregistre dans le tableau de structure
    int resultat = 0, i;
    printf("\nRecuperation des information des fichiers.\n");

    for (i = 0; i < nb_fichiers; i++) {
        resultat = stat(nFichiers[i], &infos_fichiers[i].st);
        if (resultat == -1) {
            printf("Erreur de recuperation des informations du fichier : %s\n", nFichiers[i]);
             free(infos_fichiers);
            close(fd_archive);
            exit(-1);
        }
        strcpy(infos_fichiers[i].nom, nFichiers[i]);
    }

    // J'ecris toutes les informations des fichiers dans le fichier archive 
    int taille_infos_fichiers = nb_fichiers * sizeof(FICHIER);
    nbWrite = write(fd_archive, infos_fichiers, taille_infos_fichiers);
    
    if ( nbWrite != taille_infos_fichiers) {
        perror("Erreur d'ecriture des donnees");
        free(infos_fichiers);
        close(fd_archive);
        exit(-1);
    }

    // J'ecrire le contenu de chaque fichier dans l'archive car apres extraction ont doit pouvoir les relires
    printf("\nEnregistrement des fichiers en cours.\n\n");
    char tampon[TAILLE_BUFFER]; 
    
    for (i = 0; i < nb_fichiers; i++) {
        int fd_fichier = open(nFichiers[i], O_RDONLY);
        if (fd_fichier == -1) {
            printf("Erreur d'ouverture du fichier %s\n", nFichiers[i]);
            free(infos_fichiers);
            close(fd_archive);
            exit(-1);
        }

        // Je copie le contenu du fichier vers l'archive de bloc par bloc
        int octets_lus;
        nbWrite = 0; 
        
        while ((octets_lus = read(fd_fichier, tampon, TAILLE_BUFFER)) > 0) {
            int octets_ecrits = write(fd_archive, tampon, octets_lus);
            if (octets_ecrits != octets_lus) {
                perror("Erreur d'ecriture des donnees");
                close(fd_fichier);
                free(infos_fichiers);
                close(fd_archive);
                exit(-1);
            }
            nbWrite = nbWrite + octets_ecrits;
        }

        if (octets_lus == -1) {
            perror("Erreur de lecture des donnees");
            close(fd_fichier);
            free(infos_fichiers);
            close(fd_archive);
            exit(-1);
        }

        printf("Fichier %s bien sauvegarder.\n", nFichiers[i]);
        close(fd_fichier);
    }

    free(infos_fichiers);
    close(fd_archive);
    
    printf("\nL'archive %s a ete bien creer.\n", nom_archive);
}

void Extraire_archive(char *nom_archive) {

    int fd_archive = open(nom_archive, O_RDONLY);
    if (fd_archive == -1) {
        perror("Erreur d'ouverture de l'archive.");
        exit(-1);
    }

    // Je lis le nombre de fichiers contenus dans l'archive
    int nb_fichiers;
    int nbRead = read(fd_archive, &nb_fichiers, sizeof(int));
    if (nbRead <= 0) {
        perror("Erreur de lecture du nombre de fichiers.");
        close(fd_archive);
        exit(-1);
    }
    
    printf("\nExtraction des fichiers en cours.\n\n");
    // J' alloue de la memoire pour lire les donnees des n fichiers
    FICHIER *infos_fichiers = malloc(nb_fichiers * sizeof(FICHIER));
    if (infos_fichiers == NULL) {
        perror("Pas assez de memoire");
        close(fd_archive);
        exit(-1);
    }

    // Je lis toutes les donnees en un seul bloc
    int taille_infos_fichiers = nb_fichiers * sizeof(FICHIER);
    nbRead = read(fd_archive, infos_fichiers, taille_infos_fichiers) ;

    if (nbRead != taille_infos_fichiers) {
        perror("Erreur de lors de la lecture des donnees");
        free(infos_fichiers);
        close(fd_archive);
        exit(-1);
    }

    // Extraire chaque fichier de l'archive
    char tampon[TAILLE_BUFFER];  
    
    for (int i = 0; i < nb_fichiers; i++) {
        printf("Extraction du fichier %s en cours.\n", infos_fichiers[i].nom);
        // Ici je cree le fichier de destination avec ses permissions 
        int fd_fichier = open(infos_fichiers[i].nom, O_WRONLY | O_CREAT | O_TRUNC, infos_fichiers[i].st.st_mode);
        if (fd_fichier == -1) {
            printf("Erreur lors de la creation du fichier %s.\n", infos_fichiers[i].nom);
            free(infos_fichiers);
            close(fd_archive);
            exit(-1);
        }

        // Je lis et ecris les donnees des fichier de bloc par bloc
        int restant = infos_fichiers[i].st.st_size;  
        
        while (restant > 0) {
            // Ici on determine le minimum d'octet a lire entre le buffer et les octets restants
            int a_lire = (restant < TAILLE_BUFFER) ? restant : TAILLE_BUFFER;
            int octets_lus = read(fd_archive, tampon, a_lire);
            
            if (octets_lus <= 0) {
                perror("Erreur lors de la lecture des donnees");
                close(fd_fichier);
                free(infos_fichiers);
                close(fd_archive);
                exit(-1);
            }

            // J'ecris les donnees lues dans le fichier de destination
            int octets_ecrits = write(fd_fichier, tampon, octets_lus);
            if (octets_ecrits != octets_lus) {
                perror("Erreur d'ecriture des donnees");
                close(fd_fichier);
                free(infos_fichiers);
                close(fd_archive);
                exit(-1);
            }

            restant = restant - octets_lus;  // Decrementer le nombre d'octets restants
        }

        close(fd_fichier);

        // Remettre les permissions normales
        if (chmod(infos_fichiers[i].nom, infos_fichiers[i].st.st_mode) != 0) {
            printf("Attention: impossible de restaurer les permissions de '%s'\n", 
                    infos_fichiers[i].nom);
        }
    }

    free(infos_fichiers);
    close(fd_archive);
    
    printf("\nExtraction des fichiers bien terminee.\n");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
		printf("Erreur : Nombre d'arguments insuffisant.\n");
        printf("Exemple d'usage:\n");
        printf("Creation d'archive: %s archive.l3info fichier1 fichier2 ...\n\n", argv[0]);
        printf("Extraction d'archive: %s -e archive.l3info\n\n", argv[0]);
        return 0;
    }

    if (strcmp(argv[1], "-e") == 0) {
        if (argc != 3) {
            printf("\nErreur: nombre d' argument incorrecte en usage.\n\n");
            printf("Exemple d'usage:\n");
            printf("Extraction d'archive: %s -e archive.l3info\n\n", argv[0]);
            return 1;
        }
         Extraire_archive(argv[2]);
         return 0;
    }
    
    if (argc < 3) {
        printf("\nErreur: il faut au moins archiver un fichier.\n\n");
        printf("Exemple d'usage:\n");
        printf("Creation d'archive: %s archive.l3info fichier1 fichier2 ...\n\n", argv[0]);
        return 1;
    }

    Creer_archive(argv[1], &argv[2], argc - 2);
    return 0;
}