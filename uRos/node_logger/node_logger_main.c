#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "../common_file/node.h"
#include "../common_file/signal_function.h"




//////////////////////////////////////////////////////////////////////////////////////////////////
// 				   	  NODE_LOGGER																//
//////////////////////////////////////////////////////////////////////////////////////////////////

/************************************************************************************************/
/* Ce programme est un noeud qui écoute sur un port to_log et écrit les données reçues, par le  */
/* le biais d'un fichier fifo, dans un fichier texte. Le noeud peut éventuellement republier 	*/
/* les données reçues sur un port copy_out pour les rendre disponible dans un nouveaux Fifo.	*/
/*																								*/
/* Exemple d'utilisation :																		*/
/* ./node_logger --listen to_log myfifo.fifo copy_out fifobis.fifo --logfile doc.txt> 			*/
/************************************************************************************************/

int main(int argc, char** argv){
	sigset_t signals1, signals2; //Variables permettant de gerer les signaux
	int sig = SIGINT; //Signal a surveiller
	int to_logfd, writefd; // Descripteurs
	int taille_lu1; 		// Nombre de caractère lu dans le fifo
	char buf1[SSIZE_MAX]; 	// Tampon pour réupérer le message
	param parametre; 	// Stocke les arguments du programme
	
	int continu=1;

	// Récupération des arguments du programme
  	read_arg_log(argc,argv,&parametre);

	// Vérification du bon nombre d'argument stocké
  	if(!parametre.set){
    		fprintf(stderr,"Erreur lors de l'initialisation\n");
    		return 1;
  	}

	// Lecture du Fifo du port to_log et initialisation de son descripteur
	to_logfd = read_fifo(&(parametre.listen[0]));
	if (to_logfd > 0) {
		// Création d'un nouveau fifo de recopie
		if (parametre.set == 3) {
			writefd = open_fifo(parametre.listen[1].fifo);
			if (writefd < 0) {
				parametre.set = 1;
			}
		}

		//Regle la lecture sur un mode non bloquant afin de permettre la capure de signaux
		int flags = fcntl(to_logfd, F_GETFL, 0);
		fcntl(to_logfd, F_SETFL, flags | O_NONBLOCK);

		disable_signal(&signals1, sig); //Desactive les signaux
		FILE* fic = fopen(parametre.logfile, "a");
		while (continu) {
			//did_timeout = select(FD_SETSIZE, &set, NULL, NULL, &timeout);
			taille_lu1 = read(to_logfd, buf1, SSIZE_MAX);
			// On vérifie qu'il y a bien des choses qui ont été lues
			if (taille_lu1 > 0) {
				buf1[taille_lu1] = '\0';
				// Début de la lecture et écriture dans le fichier .txt
				fprintf(fic, "%s", buf1);
				if (parametre.set == 3) {
					// Ecriture dans le fifo de recopie
					write(writefd, buf1, taille_lu1);
				}
			}
			//Verifie que la condition d'arret n'a pas ete declencher, soit du à un signal soit du à un messange dans le pipe
			if (strncmp(buf1, "kill", 4) == 0 || is_signal_trigered(&signals2, sig)) {
				continu = 0;
			}
		}
		//Ferme les descripteurs de fichier
		fclose(fic);
		close(to_logfd);
	}
	//Detruit le fifo
	unlink(parametre.listen[0].fifo);
	if(parametre.set == 3){
		close(writefd);
		unlink(parametre.listen[1].fifo);
	}  	
	reenable_signal(&signals1);// Reactive le signal
	return 0;
}
