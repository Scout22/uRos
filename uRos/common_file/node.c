#include "node.h"
#include <string.h>
#include <unistd.h>

cmd_type commande_type(char* commande) {
	float temp1, temp2;
	if (commande) {

		if (sscanf(commande, "forward %f", &temp1) == 1 && temp1>0) {
			return FORWARD;
		}
		else if (sscanf(commande, "backward %f", &temp1) == 1 && temp1>0) {
			return BACKWARD;
		}
		else if (sscanf(commande, "cw %f", &temp1) == 1 && temp1>0) {
			return CW;
		}
		else if (sscanf(commande, "ccw %f", &temp1) == 1 && temp1>0) {
			return CCW;
		}
		else if (strncmp(commande, "stop", 4) == 0) {
			return STOP;
		}
		else if (sscanf(commande, "set_v1 %f %f", &temp1, &temp2) == 2) {
			return SETV;
		}
		else if (strncmp(commande, "kill", 4) == 0) {
			return KILL;
		}

	}

	return INVALID;
}

int read_speed_from_commande(char* commande, float *vr, float *vl) {
  if(!commande || !vr || !vl){
    return -1;}
  cmd_type cmde_type=commande_type(commande);
	if (cmde_type < INVALID || cmde_type > KILL ) {
		return -1;
	}
	float temp1, temp2;
	switch (cmde_type) {
	case SETV:
		sscanf(commande, "set_v1 %f %f", &temp1, &temp2);
		*vr = temp1;
		*vl = temp2;
		break;
	case FORWARD:
		sscanf(commande, "forward %f", &temp1);
		*vr = temp1;
		*vl = temp1;
		break;
	case BACKWARD:
		sscanf(commande, "backward %f", &temp1);
		*vr = -temp1;
		*vl = -temp1;
		break;
	case CW:
		sscanf(commande, "cw %f", &temp1);
		*vr = -temp1;
		*vl = temp1;
		break;
	case CCW:
		sscanf(commande, "ccw %f", &temp1);
		*vr = temp1;
		*vl = -temp1;
		break;
	case STOP:
		*vr = 0;
		*vl = 0;
		break;

	case KILL:
		return -2;
	case INVALID:
		return -1;
		break;
	}
	return 1;
}

void treat_cmd(float* vr, float* vl, int* cont, char* buf,int taille) {
	char retour[SIZE]; // Permet de stocker la commade parsé depuis le buffer
	int cursor = 0;	  // Cette variable permet de suivre la progression dans le buf
	

	if (buf == NULL) {
		fprintf(stderr, "Error *buf is empty\n");
		return;
	}
	if (!vr || !vl) {
		fprintf(stderr, "vr or vl invalid\n");
		return;
	}

	//Deux commandes peuvent arrivé en même temps pour les traiters il faut les divisers en chaine de charactere par commande
	while (cursor<(taille-1)*4 && memccpy(retour, (buf + cursor), '\n', 25)) {
		// On stocke dans 'retour' la chaîne de caractère jusqu'à un retour à la ligne, c'est à dire toute 
		// la ligne de commande (ex: forward 25)
		
		//On applique l'action approprié vis à vis de la commande
		switch (commande_type(retour)) {
		case KILL:
			*cont = 0;
		default:
			read_speed_from_commande(retour, vr, vl);
		case INVALID:
			cursor += strlen(retour);

		}
	}
}
		
void read_arg_simu(int argc, char** argv, param* parametre){
	if(!parametre){
 		fprintf(stderr,"Error *param is empty\n");
    		return;
  	}

	parametre->set = 0;
  	int map_ok = 0, pos_ok = 0, listen_ok = 0, publish_ok = 0;
	int i;
	
	// Recherche des différents arguments
	for (i = 1; i < argc; i++) {

		// Argument --map
		if (!strcmp("--map", argv[i])) {
			if (i + 1 < argc) {
				// Test pour savoir si notre fichier a le bon type
				if (!strcmp(".pbm", strrchr(argv[i + 1], '.'))) {
					strcpy(parametre->map, argv[i + 1]);
					map_ok = 1;
					i++;
				}
			}
			else {
				display_usage(2);
				return;
			}
		}

		// Argument --commande
		else if (!strcmp("--listen", argv[i])) {
			if (i + 2 < argc && !strcmp("commande", argv[i + 1])) {

				strcpy(parametre->listen[0].port, "commande");
				strcpy(parametre->listen[0].fifo, argv[i + 2]);
				listen_ok = 1;
				i+=2;
			}
			else {
				display_usage(2);
				return;
			}
		}

		// Argument --publish
		else if (!strcmp("--publish", argv[i]) && publish_ok < 2) {
			// Port de sortie 1 (bumper_l ou bumper_r)
			if (i + 2 < argc) {
				if (!strcmp("bumper_l", argv[i + 1])) {
					strcpy(parametre->publish[0].port, argv[i + 1]);
					strcpy(parametre->publish[0].fifo, argv[i + 2]);
					publish_ok++;
					i += 2;
				}
				else if (!strcmp("bumper_r", argv[i + 1])) {
					strcpy(parametre->publish[1].port, argv[i + 1]);
					strcpy(parametre->publish[1].fifo, argv[i + 2]);
					publish_ok++;
					i += 2;
				}
				else {
					display_usage(2);
					return;
				}
			}

		}


		// Argument --pos-init
		else if (!strcmp("--init-pos", argv[i])) {
			if (i + 2 < argc) {
				parametre->posi[0] = atof(argv[i + 1]);
				parametre->posi[1] = atof(argv[i + 2]);
				pos_ok = 1;
				i += 2;
			}
			else {
				display_usage(2);
				return;
			}
			// Si aucun argument ne correspond on affiche une erreur
		}

		else {
			display_usage(2);
			return;
		}
	}
	
		// Verification du nombre d'argument initialisé
	if (map_ok && pos_ok && listen_ok) {
		parametre->set = 1 + publish_ok;
	}
	else
		{
			display_usage(2);
		}
		
}
	
void read_arg_log(int argc, char** argv, param* parametre){
	if(!parametre){
 		fprintf(stderr,"Error *param is empty\n");
    		return;
  	}
	
  	parametre->set = 0;
  	int listen_ok = 0, logfile_ok = 0, publish_ok=0;
	int i;

	// Recherche des différents arguments
	for (i = 1; i < argc; i++) {

		// Argument --listen
		if (!strcmp("--listen", argv[i]) && !listen_ok) {
			if (i + 2 < argc) {
				// Premier port d'entrée		
				strcpy(parametre->listen[0].port, argv[i + 1]);
				strcpy(parametre->listen[0].fifo, argv[i + 2]);
				listen_ok = 1;
				i += 2;
			}
			else {
				display_usage(1);
			}
		}
		//Argument --publish
		else if (!strcmp("--publish", argv[i]) && !publish_ok) {
			// Port de republication
			if (i + 2 < argc) {
				strcpy(parametre->listen[1].port, argv[i + 1]);
				strcpy(parametre->listen[1].fifo, argv[i + 2]);
				publish_ok++;
				i += 2;
			}
			else {
				display_usage(1);
			}
		}
		// Argument --logfile
    		else if(!strcmp("--logfile",argv[i])){
      			if(i+1<argc){
				strcpy(parametre->logfile,argv[i+1]);
				logfile_ok=1;
				i++;
				}
				else {
					display_usage(1);
					return;
				}
		// Si aucun argument ne correspond on affiche une erreur
    		}else{
			display_usage(1);
			return;
      		}
  	}
	
	// Verification du nombre d'argument initialisé
	if (listen_ok && logfile_ok)
		parametre->set = 2 + publish_ok;
	else
		display_usage(1);
		
}

void read_arg_pub(int argc, char** argv,param* parametre){
  	if(!parametre){
    		fprintf(stderr,"Error *param is empty\n");
    		return;
  	}
  
	parametre->set=0;
  	parametre->interval=1;
  	int path_ok=0, msg_ok=0;
  	int i;

	// Recherche des différents arguments
  	for(i=1;i<argc;i++){
		
		// Argument --msg
    		if(!strcmp("--msg",argv[i])){
      			if(i+1<argc){
				strcpy(parametre->msg,argv[i+1]);
				msg_ok=1;
				i++;
      			}
    		}

		// Argument --interval
    		else if(!strcmp("--interval",argv[i])){
      			if(i+1<argc){
				// Récupération d'un entier
				if(sscanf(argv[i+1],"%f",&parametre->interval)!=1){
	  				display_usage(0);
	  				return;
				}
				// Si le nombre est négatif ou null on affiche une erreur
				if(parametre->interval<=0){
	  				fprintf(stderr, "Le programme est causal et ne peut pas emmettre un message toutes les 0 secondes\n");
	  				return;
				}
	 			i++;
      			}else{
				display_usage(0);
				return;
      			}
    		}
    
		// Argument --publish
		else if(!strcmp("--publish",argv[i])){
      			if(i+2<argc){
				strcpy(parametre->publish[0].port,argv[i+1]);
				strcpy(parametre->publish[0].fifo,argv[i+2]);
				path_ok++;
				i+=2;
      			}else{
	  			display_usage(0);
	  			return;
			}
		// Si aucun argument ne correspond on affiche une erreur
    		}else{
			display_usage(0);
			return;
      		}
  	}
 
	// Verification du nombre d'argument initialisé
	if(path_ok && msg_ok)
    		parametre->set=1;
}

void read_arg_tele(int argc, char** argv, char* fifo){
	
	if (argc == 4 && argv && fifo && !strcmp(argv[1], "--publish") && !strcmp(argv[2], "commande_out") ) {
		strcpy(fifo, argv[3]);
		return;
	}
	else {
		fifo[0] = '\0';
	}
}

int open_fifo(char *fifo){
  	int fd_output;

	//Création du Fifo dont le nom est dans fifo
  	if(mkfifo(fifo,PERMISSION)<0 && (errno != EEXIST) ){
		fprintf(stderr,"Impossible de crée le tube \"%s\"\n",fifo);
    		return -1;
    	}
	// Ouverture du fifo et initialisation du descripteur fd_output en mode non bloquant
	if ((fd_output = open(fifo, O_WRONLY)) < 0) {
		printf("Impossible d'ouvrir la fifo pour l'ecriture des données\n");
		return -1;
	}
	
	fcntl(fd_output, F_SETFL, O_WRONLY); //Rend l'ecriture bloquante
  	return fd_output;
}

int read_fifo(reader* listen){
	int fd;

	//Création du Fifo dont le nom est dans listen->fifo
  	if(mkfifo(listen->fifo,PERMISSION)<0 && (errno != EEXIST) ){
		fprintf(stderr,"Impossible de crée le tube %s\n",listen->fifo);
    		return -1;
    	}

	// Les FIFOs de lecture ont déjà été créé par le Noeud node_echo
	// Ouverture du fifo et initialisation du descripteur fd_output
	if ((fd = open(listen->fifo, O_RDONLY | O_NONBLOCK)) < 0){
		fprintf(stderr,"Ne parvient pas a ouvrir le descripteurs en mode lecture\n");
		return -1;
	}
	fcntl(fd, F_SETFL, O_RDONLY); //Rend la lecture bloquante
	return fd;
}

void display_usage(int type){
	switch (type){
		case 0:
  			fprintf(stderr,"Usage: ./node_echo --publish nom_du_port chemin_de_la_fifo_lie_au_port --msg Message_a_transmettre \n (--interval duree_en_seconde) \n");
			break;
		case 1:
	  		fprintf(stderr,"Usage: ./node_logger --listen input inputpath.fifo --logfile <nom_du_fichier.txt> \n(--publish output outputpath.fifo) \n");
			break;
		case 2:
			fprintf(stderr,"Usage: ./node_simu --listen commande commande.fifo --map <fichier_carte.bmp> --init-pos <x_initial> <y_initial> \n(--publish bumper_r bumper_r.fifo --publish bumper_l bumper_l.fifo)\n");
			break;
		case 3:
			break;
		case 4:
			fprintf(stderr, "Usage: ./node_tele --publish out output.fifo");
			break;
		default:
			break;
	}
}

void init_capteur(capteur* capteurs){
	capteurs->vl = 0;
	capteurs->vr = 0;
	capteurs->bumper_r = 0;
	capteurs->bumper_l = 0;
	capteurs->continu = 1;
	// Initialisation des Mutex
	capteurs->mutex_cmd = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
	capteurs->mutex_bump = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;

}

void* commande(void* arg){
	int taille_lu = 0, continu = 1; // Permet de savoir le nombre de caractères lu dans le fifo
	char buf[SSIZE_MAX]; // Stocke les caractères lus dans le fifo
	capteur* listen = (capteur*) arg;

	while(continu){
		// Début de la lecture de la fifo de commande
    		taille_lu = read(listen->desc[0], buf, SSIZE_MAX);

		// On vérifie qu'il y a bien des choses qui ont été lues
		if(taille_lu){
			// Verrouillage du mutex commande
			pthread_mutex_lock(&(listen->mutex_cmd));

			treat_cmd(&(listen->vr), &(listen->vl),&(continu), buf,taille_lu);
			listen->continu = continu;
			// Dévérouillage du mutex commande
			pthread_mutex_unlock(&(listen->mutex_cmd));
		}
	}
	pthread_exit(0);
}

void* bumper(void* arg){
	int continu = 1; // Permet de savoir le nombre de caractères lu dans le fifo
	capteur* listen = (capteur*) arg;
	while(continu){
		// Verrouillage du mutex bumper
		pthread_mutex_lock(&(listen->mutex_bump));

		// On écrit d'abord l'état du bumper gauche puis celui du droit
		write(listen->desc[1], &(listen->bumper_l), sizeof(char));

		write(listen->desc[2], &(listen->bumper_r), sizeof(char));

		// Dévérouillage du mutex bumper
		pthread_mutex_unlock(&(listen->mutex_bump));
		usleep(PERIODE_PUB_B);
	}
	pthread_exit(0);
}
