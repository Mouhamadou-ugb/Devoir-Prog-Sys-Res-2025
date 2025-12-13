#include <stdio.h>      
#include <sys/types.h> 
#include <unistd.h>     
#include <string.h>    
#include <sys/wait.h>   
#include <stdlib.h>    

void Executer(char *argv[]);

int main(int argc, char const *argv[])
{
    char cmd[255]; 
    char *commandes[255];  // Pour stocker chaque commande separee par ;
    char *argvExec[255];  
    int i, j;

    do {
        printf("Minishell > ");
        
        fgets(cmd, sizeof(cmd), stdin);  // mon systeme refuse le gets

        cmd[strcspn(cmd, "\n")] = '\0';  // supprimer le \n quand l'user appuie sur entree sinon on a l' erreur no such file ...

        if (strcmp(cmd, "quit") == 0)  // si l'user appuie sur quit, on arrete  
            break;
        
        for (i = 0; i < 255; i++)
            commandes[i] = NULL;  // J'initialise a nulle par securite pour stocker les cmd separee par ;
        
        commandes[0] = strtok(cmd, ";");
        
        // Je decoupe par les ; pour obtenir chaque cmd
        i = 0;
        while (commandes[i] != NULL) {
            i++;
            commandes[i] = strtok(NULL, ";");
        }
        
        // Je decoupe chaque commande et l'execute
        i = 0;
        while(commandes[i] != NULL) {
            for (j = 0; j < 255; j++)
                argvExec[j] = NULL;
            
            // Je prend une commande a executer 
            char *cmd_actuelle = commandes[i];
            argvExec[0] = strtok(cmd_actuelle, " "); 
            j = 0;
            while (argvExec[j] != NULL) { 
                j++;
                argvExec[j] = strtok(NULL, " ");  // recuperation des options
            }
            
            // On execute la cmd si elle est non nulle
            if (argvExec[0] != NULL) 
                Executer(argvExec);
                
            i++;
        }
        
    } while(1);
    
    return 0; 
}

void Executer(char *argv[]) {
    pid_t pidfils; 
    
    pidfils = fork();
    
    switch (pidfils)
    {
    case -1: 
        perror("Erreur de creation du processus");
        break; 
        
    case 0: 
        execvp(argv[0], argv);
        perror("Erreur d'execution de la commande");
        exit(-1);
        break;
        
    default:
        wait(NULL); 
    }
}