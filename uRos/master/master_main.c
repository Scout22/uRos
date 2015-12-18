#include "master.h"
#include "../common_file/signal_function.h"
/*
Master permet de lire un fichier de configuration, de tester sa validité, lancer les
les executables avec les bons arguments et de s'assurer de leur fermeture à la fin  
de l'execution.
Pour se faire le master crée des processus fils dont le PID est enregistré, et lors -
que la condition d'arrete (SIGINT: Ctrl-C) est reçu envoie un signal d'arrete au di -
fferent processus
. / master config.txt
config.txt un fichier de configuration formaté definissant les executables à lancer
et les parametres associés.
*/
int main(int argc,char *argv[]){
	sigset_t signals1, signals2;
	if (argc!=2){
		fprintf(stderr, "usage: ./master config_file.txt\n");
		return 1;
	}
	node_list* nd_list=malloc(sizeof(node_list));
	if (!nd_list) {
		fprintf(stderr, "Erreur lors de l'allocation de la memoire\n");
		return -1;
	}
	if (parse_config(argv[1], nd_list)<=0) {
		return -1;
	}
	display_config(nd_list); //affichage de la liste de programme a executer
	launch_all_node(nd_list); //lancement des programme
	disable_signal(&signals1, SIGINT); //desactive le signal associé à ctrl-C
	fprintf(stdout, "press Ctrl-C to kill all node\n");
	while (!is_signal_trigered(&signals2, SIGINT) && !waitpid(-1, NULL, WNOHANG)) { //tant que le signal associé à ctrl-C n'est pas declenché et que des noeuds
																					//sont encore actif attendre
		sleep(1);
	}
	sleep(1); //Laisse le temps au processus ayant declencher l'arret de s'eventuellement arreter
	fprintf(stdout, "Ctrl-C pressed, killing all node...\n");
	kill_all_node(nd_list);//Demande à tous les enfant de mourrir
	//Attend la mort de tout les enfant
	while (waitpid(-1, NULL, 0)) {
		if (errno == ECHILD) {
			break;
		}
	}
	detruire_liste_noeud(nd_list);
	fprintf(stdout, "All node killed!\n");
	reenable_signal(&signals1);// Reactive le signal
	return 0;
}
