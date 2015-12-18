#ifndef MASTER_H
#define MASTER_H
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#define SIGNAL SIGINT
#define SIZE 256 //Taille des buffers servant à parser des informations
typedef enum way{PUBLISH,LISTEN,UNSET} way;
typedef enum line_type{NAME,BINARY,LPUBLISH,LLISTEN,ARGS,INVALID} line_type;

//Structure permettant de stocker une liste de FIFO
typedef struct fifo{
	char *fifo_port; //Port du fifo
	char *fifo_path; //Chemin du fifo
	way type; //Direction de la fifo (PUBLISH ou LISTEN)
	struct fifo * next_fifo; //Fifo suivante de la liste chainé
}fifo;

//Structure permettant de stocker les variables lié un programme noeud
typedef struct node{
	char* name;			//Nom du noeud
	char* bin;			//Chemin de l'executable
	char** args;		//Liste d'arguments d'execution
	int nb_args;		//nombre d'argument
	int pid;			//PID du programme une fois lancé
	fifo* fifo_list;	//Liste des differentes fifo associé au programme
} node;

//Structure permettant de stocker une liste de noeud
typedef struct node_list{
	node** nd_list;	//Liste de noeud
	int nb_node;	//Nombre de noeud dans la liste
} node_list;
//------------------------------------------------------------------------------//
// Cette fonction permet de tuer tout les programme noeud present				//
// dans la liste list passé en argument											//
//------------------------------------------------------------------------------//
void kill_all_node(node_list* list);
//------------------------------------------------------------------------------//
// Effectue la conversion d'une liste d'argument en string	formaté				//
//------------------------------------------------------------------------------//
void list_arg_to_string(char** arg_list,int nb_args, char* string);
//------------------------------------------------------------------------------//
// Effectue la conversion d'un string contenant des	argument en liste			//
// d'argument terminé par NULL													//
//------------------------------------------------------------------------------//
void string_to_arg_list(char *** args_list, int* nb_args, char * string_arg);
//------------------------------------------------------------------------------//
// Cette fonction copie une chaine de charactere in_string et retourne			//
// un pointeur vers la copie													//
//------------------------------------------------------------------------------//
char* copy_string(char* in_string);
//------------------------------------------------------------------------------//
// Cree une varaiable noeud à partir des arguments passé à la fonction			//
// retourne un pointeur vers le noeud crée										//
//------------------------------------------------------------------------------//
node* initialiser_noeud(char* name, char* bin, char* args, fifo* fifo_list);
//------------------------------------------------------------------------------//
// Cette fonction determine le type de ligne que l'on est entrain de lire		//
// dans un fichier confi, se referer à la definition de line_type pour			//
// la liste des retour possible													//
//------------------------------------------------------------------------------//
line_type determine_line_type(char* string);
//------------------------------------------------------------------------------//
// Cette fonction detruit une structure de type noeud							//
// et libere la memoire associé													//
//------------------------------------------------------------------------------//
void detruire_noeud(node* noeud);
//------------------------------------------------------------------------------//
// Cette fonction detruit une structure de type node_list						//
// et libere la memoire associé													//
//------------------------------------------------------------------------------//
void detruire_liste_noeud(node_list* nd_list);
//------------------------------------------------------------------------------//
// Cette fonction sert a lire les arguments passé au preogramme					//
// et reccuperer le chemin du ficher de configuration							//
// Retourne le nombre de noeud correctement lu									//
//------------------------------------------------------------------------------//
int parse_config(char* path_name, node_list* nd_list);
//------------------------------------------------------------------------------//
// Fonction permettant de faire une affichage graphique							//
// d'une liste de noeud															//
//------------------------------------------------------------------------------//
void display_config(node_list* config);
//------------------------------------------------------------------------------//
// Fonction permettant de compter le nombre de noeud present					//
// dans un fichier, à partir du descripteur de fichier associé					//
//------------------------------------------------------------------------------//
int count_node_in_file(FILE* config);
//------------------------------------------------------------------------------//
// Cette fonction lance l'execution de l'ensemble des noeuds present			//
// dans la liste passé en argument												//
//------------------------------------------------------------------------------//
void launch_all_node(node_list* list);
//------------------------------------------------------------------------------//
// Cette fonction lance l'execution de du noeuds decrit par noeud				//
// et indique le PID dans le champ PID											//
//------------------------------------------------------------------------------//
void launch_node(node* noeud);
#endif
