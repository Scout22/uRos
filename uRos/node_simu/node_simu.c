#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include "../common_file/node.h"
#include "../common_file/fastsim/fastsim_c.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////
// 				   	  						NODE_SIMU												//
//////////////////////////////////////////////////////////////////////////////////////////////////////

/****************************************************************************************************/
/* Ce programme est un noeud qui permet d'utiliser l'environnement fastsim. Il cré donc un			*/
/* robot à la position (x,y) spécifié par --init-pos, une carte dont le nom est passé grâce à		*/
/* --map, et enfin il s'occupe de l'affichage de l'ensemble.										*/
/* Une fois que tout a été initialisé, le noeud lance la simulation. Pour ce faire, il écoute 		*/
/* sur le port --commande les commandes qui sont écrites dans le fifo. Une fois la commande 		*/
/* reçue, node_simu va identifier la commande tappé, et raffraichir le simulateur en fonction		*/
/* de la commande. Les commandes sont:																*/
/*		- forward <vitesse> : met les deux vitesses des moteurs à <vitesse>							*/
/*		- backward <vitesse> : met les deux vitesses des moteurs à -<vitesse>						*/
/*		- cw <vitesse> : fait tourner le robot dans le sens des aiguilles d'une montre				*/
/*		- ccw <vitesse> : de même que cw mais dans l'autre sens										*/
/*		- stop : arrête le robot																	*/
/*		- set_v v1 v2 : met le moteur droite à v1 et le moteur gauche à v2							*/
/* De plus le noeud permet aussi d'envoyer sur deux fifos différents, l'état des capteurs de		*/
/* proximités qui permettent de détecter les obstacles. Il enverra sur le port bumper_l l'état		*/
/* du bumper gauche (1 active, 0 sinon) et le port bumper_r l'état du bumper droit.					*/
/*																									*/
/* Exemple d'tilisation :																			*/
/* LD_PRELOAD=fastsim/libfastsim.so ./node_simu --commande myfifo.fifo --map <carte.pbm> --init-p	*/
/* os <x_initial> <y_initial> --publish bumper_l bl.fifo bumper_r br.fifo							*/
/****************************************************************************************************/

int main(int argc, char* argv[]){

	pthread_t thread[2];	// Tableau contenant les 2 threads commande et bumper
	capteur* capt = (capteur*)malloc(sizeof(capteur)); // Ensemble des variables capteurs
	int continu = 1; // Permet de sortir de la boucle infinie
	int i = 0; // Pour compter le nombre d'itération de la boucle infinie
	param parametre; // Variable pour stocker les paramètre placé en argument du programme

	// Initialisation des capteurs
	init_capteur(capt);

	// Récupération des arguments du programme
	read_arg_simu(argc, argv, &parametre);

	// Vérification du bon nombre d'argument stocké
	if (!parametre.set) {
		fprintf(stderr, "Erreur lors de l'initialisation\n");
		return -1;
	}

	// Création du robot à la position pacé en argument du programme
	robot_t *rob = new_robot(parametre.posi[0], parametre.posi[1]);
	
	// Création de la map dont le nom est passé en paramètre du programme
  	map_t *map = new_map(parametre.map);

	// Création de l'affichage
  	display_t *disp = new_display(map, rob);

	// Ouverture du fifo dont le nom est passé en paramètre du programme en mode lecture
	capt->desc[0] = read_fifo(&(parametre.listen[0]));
	if (capt->desc[0] > 0) {

		// Création du Thread de commande
		if (pthread_create(&thread[0], NULL, commande, (void*)capt)) {
			printf("Erreur lors de la création du thread de commande dans node_simu\n");
		}

		if (parametre.set == 3) {
			// Ouverture des Fifos bumper
			capt->desc[1] = open_fifo(parametre.publish[0].fifo);
			capt->desc[2] = open_fifo(parametre.publish[1].fifo);
			if (capt->desc[1] < 0 || capt->desc[2] < 0) {
				exit(-1);
			}
			// Création du Thread des bumpers
			if (pthread_create(&thread[1], NULL, bumper, (void*)capt))
				printf("Erreur lors de la création du threads des bumpers dans node_simu\n");
		}
		else {
			capt->desc[1] = 0;
			capt->desc[2] = 0;
		}

		while (continu) {
			//////////////////////////////////////////////////
			//		BLOC MUTEX BUMP						    //
			//////////////////////////////////////////////////
				// Verrouillage du mutex bumper
			pthread_mutex_lock(&(capt->mutex_bump));

			// Update buffer
			//La valeur 48 permet de reccuperer le code ascii pour le charactere 0 et 1
			capt->bumper_l = 48 + get_left_bumper(rob);
			capt->bumper_r = 48 + get_right_bumper(rob);

			// Dévérouillage du mutex bumper
			pthread_mutex_unlock(&(capt->mutex_bump));

			///////////////////////////////////////////////////
			//		BLOC MUTEX CMD						     //
			///////////////////////////////////////////////////
			// Verrouillage du mutex commande
			pthread_mutex_lock(&(capt->mutex_cmd));
			// Verifie que le programme n'a pas recu la commande kill
			continu = capt->continu;
			// Déplace le robot  
			move_robot(rob, map, capt->vr, capt->vl);

			// Dévérouillage du mutex commande
			pthread_mutex_unlock(&(capt->mutex_cmd));


			// Rafraichissement de l'affichage
			update_display(disp);
#ifdef DEBUG
			// Print debug info
			printf("timestep %d ms: left bump = %c   right bump = %c  vr = %f vl = %f\n", i*PERIODE / 1000, capt->bumper_l, capt->bumper_r, capt->vr, capt->vl);
#endif
			// Sleep to make the simulation slow enough for humans
			i++;
			usleep(PERIODE);
		}

		// Libère la mémoire occupée par les structures 
		delete_display(disp);
		delete_map(map);
		delete_robot(rob);

		// Suppression des mutex
		pthread_mutex_destroy(&(capt->mutex_cmd));
		pthread_mutex_destroy(&(capt->mutex_bump));


		// Fermeture des descripteurs
		close(capt->desc[0]);
		if (parametre.set == 3) {
			// On tue le thread lié au bumper
			pthread_cancel(thread[1]);
			//On ferme les descripteur
			close(capt->desc[1]);
			close(capt->desc[2]);
			unlink(parametre.listen[1].fifo);
			unlink(parametre.listen[2].fifo);
		}
	}
	unlink(parametre.listen[0].fifo);
	free(capt);	
	return 0;
}		
