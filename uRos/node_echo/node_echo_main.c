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
// 				   	  NODE_ECHO																	//
//////////////////////////////////////////////////////////////////////////////////////////////////

/************************************************************************************************/
/* Ce programme est un noeud qui envoie à intervalle régulier un message texte --msg sur un		*/
/* port out_port. Par defaut le programme envoie un message toutes les secondes, mais cet		*/
/* interval peut être modifié par un argument --interval en secondes.							*/
/* SIGINT provoque l'arret du programme (ctrl-C)												*/
/* Exemple d'utilisation :																		*/
/* ./node_echo --publish out_port myfifo.fifo --msg Hello --interval 15							*/
/************************************************************************************************/

int main(int argc, char** argv){
	sigset_t signals1, signals2; //Variables permettant de gerer les signaux
  	param parametre; // Stocke les arguments du programme
	int fd;	// Descripteur pour ouvrir le Fifo

	// Récupération des arguments du programme
  	read_arg_pub(argc,argv,&parametre);

	// Vérification du bon nombre d'argument stocké
  	if(!parametre.set){
  	  	fprintf(stderr,"Erreur lors de l'initialisation\n");
    		return 1;
  	}
	disable_signal(&signals1, SIGNALSTOP);
	// Ouverture du Fifo et initilisation du descripteur
	fd = open_fifo(parametre.publish[0].fifo);
	if (fd > 0) {
		// Envoie du message tous les 'interval' secondes
		while (!is_signal_trigered(&signals2, SIGNALSTOP)) {
			write(fd, parametre.msg, strlen(parametre.msg));
			sleep(parametre.interval);
		}
		close(fd);
	}
	unlink(parametre.publish[0].fifo);
	reenable_signal(&signals1);
  	return 0;
}
