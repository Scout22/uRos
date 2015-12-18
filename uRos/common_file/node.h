#ifndef NODE_H
#define NODE_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/errno.h>
#include <pthread.h>

#define PERMISSION 0777
#define SSIZE_MAX 5054 //Taille de la fifo
// Pas de temps de lecture en secondes dans node_simu.C
#define PERIODE 30000  //30ms soit 33fps
#define PERIODE_PUB_B 100000 //Bumper publish rate 100ms intervalle de temps entre deux publications de bumper
#define SIZE 256 //Taille des buffers servant à parser des information



//------------------------------------------------------------------------------//
// Strucutre gerant les paramètres des programmes Node_logger et Node_echo	//
//------------------------------------------------------------------------------//
typedef struct reader{
	char port[SIZE];    // Récupère le nom du Port
	char fifo[SIZE];    // Récupère le nom du Fifo
}reader;

typedef struct param{
	reader listen[2];  // Paramètre des ports d'entrée 2 maximum
	char logfile[SIZE]; // Récupère le nom du fichier.txt
	reader publish[2];    // Paramètre des ports de sortie 2 maximum
	char msg[SIZE]; 	   // Message a publié
	float interval;    // Interval de temps entre deux publication en secondes
	int set; 	   // Flag permettant d'indiquer si la structure a ete correctement rempli
	char map[SIZE];	   // Stock le nom de la map
	double posi[2];	   // Stock la position initiale du Robot
}param;

typedef struct capteur{
	float vr; // Commande de vitesse pour la roue droite
	float vl; // Commande de vitesse pour la roue gauche
	char bumper_r; // Bumper droit pour détecter les collisions (1 = active, 0 = inactive)
	char bumper_l; // Bumper gauche pour détecter les collisions (1 = active, 0 = inactive)
	int desc[3]; // descripteur pour lire les FIFO (0 pour les commandes de vitesse et 1 pour le bumper gauche et 2 pour le bumper droit)
	int continu;
	// Déclaration des mutex pour les 2 Threads, commande et bumper
	pthread_mutex_t mutex_cmd;
	pthread_mutex_t mutex_bump;

}capteur;

typedef enum cmd_type { INVALID,FORWARD, BACKWARD, CCW, CW, STOP,SETV,KILL } cmd_type; //Structure definissant les differentes commandes valide

//------------------------------------------------------------------------------//
// Cette fonction permet d'expliquer comment utiliser le programme, notamment	//
// quels sont les paramètres à passer en argument du programme.			//
// La fonction opère donc un affichage dans stderr qui explique comment se 	//
// servir du programme logger (type = 1), echo (type = 0), simu (type = 2).	//
//------------------------------------------------------------------------------//
void display_usage(int type);

//------------------------------------------------------------------------------//
// Cette fonction permet d'initialiser le descripteur du fifo pointé par 	//
// listen->fifo. La fonction retourne donc le descripteur ainsi initialisé.	//
// en cas d'erreur la fonction retourne -1.					//
//------------------------------------------------------------------------------//
int read_fifo(reader* listen);


//------------------------------------------------------------------------------//
// Open fifo permet de créer le fifo dont le nom est stocké dans fifo.	//
// De plus elle renvoie le descripteur qui pointe sur ce nouveau fifo.		//
// La fonction retourne -1 en cas d'erreur.					//
//------------------------------------------------------------------------------//
int open_fifo(char* fifo);



//------------------------------------------------------------------------------//
// Cette fonction permet de stocker les paramètres passés en argument du 		//
// programme dans la variable paramètre. Cette fonction ne modifiera que les	//
// paramètres de la structure ayant un rapport avec le logger, c'est à dire 	//
// listen et logfile.															//
// Listen[0] récupère le port to_log et listen[1] le port copy_out.				//
// La varible set permet de vérifier la bonne initalisation de notre variable	//
// parametre.																	//
//------------------------------------------------------------------------------//
void read_arg_log(int argc, char** argv, param* parametre);



//------------------------------------------------------------------------------//
// Cette fonction permet de stocker les paramètres passés en argument du 		//
// programme dans la variable paramètre. Cette fonction ne modifiera que les	//
// paramètres de la structure ayant un rapport avec echo, c'est à dire 			//
// publish et msg et interval.													//
// La varible set permet de vérifier la bonne initalisation de notre variable	//
// parametre.																	//
//------------------------------------------------------------------------------//
void read_arg_pub(int argc, char** argv,param* parametre);



//------------------------------------------------------------------------------//
// Cette fonction permet de stocker les paramètres passés en argument du 		//
// programme dans la variable paramètre. Cette fonction ne modifiera que les	//
// paramètres de la structure ayant un rapport avec simu, c'est à dire 			//
// publish, map, posi, listen.													//
// en ce qui concerne de map, la fonction vérifie la conformité du type du 		//
// fichier.																		//
// La varible set permet de vérifier la bonne initalisation de notre variable	//
// parametre.																	//
//------------------------------------------------------------------------------//
void read_arg_simu(int argc, char** argv, param* parametre);

//------------------------------------------------------------------------------//
// Cette fonction permet de stocker les paramètres passés en argument du 		//
// programme dans de parser l'adresse de la fifo du port de commande			//
// pour le programme node_tele													//
//------------------------------------------------------------------------------//
void read_arg_tele(int argc, char** argv, char* fifo);





//------------------------------------------------------------------------------//
// Initialise les paramètres vl, vr, bumper_l, bumper_r et les 2 mutex de la 	//
// structure capteur.															//
//------------------------------------------------------------------------------//
void init_capteur(capteur* capteurs);

//------------------------------------------------------------------------------//
// Cette fonction permet de lire une fifo et mettre à jours les vitesses 
// en conscequence																//
// Cette fonction sera passé en argument du thread de commande. Ainsi on pourra	//
// actualiser les valeurs des vitesses vl et vr sans que la lecture du FIFO ne 	//
// bloque l'execution du main qui permet de faire bouger le robot.				//
//------------------------------------------------------------------------------//
void* commande(void* arg);

//------------------------------------------------------------------------------//
// Cette fonction initialise les vitesse vr et vl en fonction					//
// de la commande passé en argument												//
//------------------------------------------------------------------------------//
int read_speed_from_commande(char* commande, float *vr, float *vl);

//------------------------------------------------------------------------------//
// Cette fonction publie sur une Fifo l'etat du bumper							//
// Cette fonction sera passé en argument du thread de bumper. Ainsi on pourra	//
// écrire dans deux FIFOS différents l'état des bumpers au temps t. Cette		//
// fonction permet de ne pas bloquer l'éxecution du programme selon qu'il y ait //
// ou non des lecteurs des FIFOs dans lesquels on souhaite écrire.				//
//------------------------------------------------------------------------------//
void* bumper(void* arg);

//------------------------------------------------------------------------------//
// Cette fonction permet de lire les commandes qui se trouvent dans buf pour	//
// initialiser les variables qui gèrent la vitesses des moteurs.				//
// Pour la liste des commandes, se référer au descriptif du programme.			//
//------------------------------------------------------------------------------//
void treat_cmd(float* vr, float* vl, int* cont, char* buf, int taille);


//------------------------------------------------------------------------------//
// Cette fonction teste si la commande saisie est valide						//
// Si c'est le cas retourne le type de commande et indique les vitesses lu		//
//------------------------------------------------------------------------------//
cmd_type commande_type(char* commande);



#endif
