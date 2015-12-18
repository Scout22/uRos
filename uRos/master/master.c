#include "master.h"


//Fonction permettant de compter lel nombre de mot
int word_count(char* string){
	//Verifie que la string existe
	if (!string) {
		return -1;
	}
	int wc = 1,i=0;
	//tant que la ligne n'est pas finie on compte
	while (string[i] != '\n' && string[i] != '\0'){
		//Si on rencontre un espace on incremente le nombre de mot de 1
		if (string[i]==' '){
			wc++;
		}
		i++;
	}
	return wc;
}


void string_to_arg_list(char *** args_list, int* nb_args, char * string_arg){
	if (args_list && nb_args && string_arg ) {
		*nb_args = word_count(string_arg); //Compte le nombre de mot equivalent au nombre d'argument
		char **tab = malloc(*nb_args*sizeof(char*));
		char *arg=strtok(string_arg, " ");//reccupere le premier mot de la chaine de charactere string_arg
		int i = 0;
		
		while (arg) {
			tab[i] = copy_string(arg); //Copie l'argument dans la case correspondate à l'indice
			arg = strtok(NULL, " "); //reccupere le mot suivant de la chaine de charactere de string_arg
			i++;
		}
		*args_list = tab;
	}
}
node* initialiser_noeud(char* name, char* bin, char* args, fifo* fifo_list){
	if (!(name && bin && fifo_list && args)){
		fprintf(stderr,"Argument d'initalisation invalide du noeud \n");
		return NULL;
	}
	node* noeud = malloc(sizeof(node));
	noeud->pid = -1;
	if (!noeud){
		fprintf(stderr, "Erreur lors de l'allocation de node \n");
		return NULL;
	}
	string_to_arg_list(&(noeud->args), &(noeud->nb_args), args); //Conevertie les arguments en une liste utilisable par exec
	noeud->bin = copy_string(bin); //Reccupere le nom de l'executable
	noeud->fifo_list = fifo_list;
	noeud->name = copy_string(name);

	if (!(noeud->name && noeud->bin && noeud->args)){
		exit(1);
	}
	return noeud;
}
char* copy_string(char* in_string){
	char *out_string = malloc((strlen(in_string)+1)*sizeof(char));
	if (!out_string){
		fprintf(stderr, "Erreur lors de l'allocation du string\n");
		return NULL;
	}
	strcpy(out_string, in_string);
	return out_string;
}
fifo* detruire_fifo(fifo* fifo_sup){
	if (fifo_sup){
		fifo* tmp_sup = fifo_sup->next_fifo;
		free(fifo_sup->fifo_path);
		free(fifo_sup->fifo_port);
		free(fifo_sup);
		return tmp_sup;
	}
	return NULL;
}
void detruire_noeud(node* noeud) {
	if (noeud) {
		fifo* tmp_list = noeud->fifo_list;
		int i;
		//Libere la liste chainé contenant les fifo
		while (tmp_list) {
			tmp_list = detruire_fifo(tmp_list);
		}
		//Libere la liste d'argument
		for (i = 0; i < noeud->nb_args; i++) {
			free(noeud->args[i]);
		}
		free(noeud->args);
		free(noeud->bin);
		free(noeud->name);
		free(noeud);
	}
}

void detruire_liste_noeud(node_list* nd_list){
	int i;
	if (nd_list) {
		//Detruit les noeuds contenu dans la liste
		if (nd_list->nd_list) {
			for (i = 0; i < nd_list->nb_node; i++) {
				detruire_noeud(nd_list->nd_list[i]);
			}
			free(nd_list->nd_list);
		}
		free(nd_list);
	}
}
line_type determine_line_type(char* string){
	if (!string){
		fprintf(stderr,"Impossible de determiner le type de ligne\n");
		return INVALID;
	}
	//Ligne indiquant un nom
	if (!strncmp(string, "node:", 5)){
		return NAME;
	}
	//Ligne indiquant un binaire
	else if (!strncmp(string, "  bin:", 5)){
		return BINARY;
	}
	//Ligne indiquant une fifo de sortie
	else if (!strncmp(string, "  publish:", 9)){
		return LPUBLISH;
	}
	//Ligne indiquat une fifo d'entrée
	else if (!strncmp(string, "  listen:", 8)){
		return LLISTEN;
	}
	//Ligne indiquant des argument
	else if (!strncmp(string, "  args:", 6)){
		return ARGS;
	}
	return INVALID;


}
int count_node_in_file(FILE* config){
	if (!config){
		fprintf(stderr, "Descripteur FILE de config invalid\n");
		exit(2);
	}
	int nb_node=0;
	rewind(config); //Remonte au debut du fichier
	char buffer[SIZE];
	while (fgets(buffer, SIZE, config)){
		//Compte le nombre de ligne de type name
		if (determine_line_type(buffer)==NAME){
			nb_node++;
		}
	}
	return nb_node;
}

fifo* add_fifo_to_list(fifo* fifo_list, char* fifo_port, char* fifo_path, way inout){
	fifo* tmp_fifo = malloc(sizeof(fifo));
	tmp_fifo->next_fifo = fifo_list;
	tmp_fifo->fifo_path =copy_string(fifo_path);
	tmp_fifo->fifo_port = copy_string(fifo_port);
	tmp_fifo->type = inout;
	return tmp_fifo;
}

void list_arg_to_string(char** arg_list, int nb_args, char* string){
	if (!string || !arg_list){
		fprintf(stderr, "Erreur lors de la concatenation \n");
		exit(4);
	}
	int i = 0;
	string[0] = '\0';
	//Concatene progressivement la liste d'argument
	for (i = 0; i < nb_args; i++){
		sprintf(string, "%s %s", string, arg_list[i]);
	}
}

int parse_config(char* path_name, node_list* nd_list){
	int i,ln_num=0; //ln_num permet de compter le nombre de ligne lu et d'indiquer la ligne posant probleme en cas d'erreur
	char buffer[SIZE],args[SIZE],bin[SIZE],fifo_path[SIZE],fifo_port[SIZE],name[SIZE];
	fifo* temp_fifo_list=NULL;
	int cont = 1,name_ok=0,binary_ok=0;

	if (!(path_name && nd_list)){
		fprintf(stderr, "Path ou nd_list invalid\n");
		exit(2);
	}
	FILE * fd = fopen(path_name, "r"); //Ouvre le fichier pointer par path
	//S'assure de l'ouverture
	if (!fd){
		fprintf(stderr, "Impossible de lire %s \n", path_name);
		exit(3);
	}
	//compte le nombre de noeud present dans le fichier
	int nb_node = count_node_in_file(fd);
	if (nb_node < 1){
		fprintf(stderr,"No node found in file %s\n", path_name);
		return 0;
	}
	//initialise la structure en fonction du nombre de noeud present
	nd_list->nd_list = malloc(nb_node*sizeof(node*));
	nd_list->nb_node = 0;
	//revient au debut du fichier
	rewind(fd);
	//Lit la premiere ligne
	fgets(buffer, SIZE, fd);
	ln_num++;
	for (i = 0; i < nb_node; i++){
		//Initialise les valeurs du noeud
		temp_fifo_list = NULL;
		name_ok = 0;
		args[0] = '\0';
		binary_ok = 0;
		cont = 1;
		//verifie que la premiere ligne contient le nom d'un noeud
		if (determine_line_type(buffer)!=NAME){
			fprintf(stderr, "Fichier invalide \n");
		}
		//lit le nom du noeud
		if (sscanf(buffer, "node: %s", name) != 1){
			break;
		}
		name_ok++;
		//Lit les parametres lié à ce noeud
		do{
			//lit la ligne suivante
			if (!fgets(buffer, SIZE, fd)){
				cont = 0;
				break;
			}
			ln_num++;
			//determine le type de ligne et traite en fonction
			switch (determine_line_type(buffer)){
			
			case ARGS: //cas d'une ligne contenant les arguments de la fonction
				strcpy(args, buffer + 7);
				args[strlen(args) - 1] = '\0'; //Evite les retours a la ligne a la fin de args
				break;
			case NAME: //cas d'une ligne contenant un nom de noeud
				cont = 0;
				break;
			case LLISTEN: //cas d'une ligne decrivant un port d'ecoute
				if (sscanf(buffer, "  listen: %s <- %s", fifo_port, fifo_path)!=2){
					cont = 0;
					break;
				}
				temp_fifo_list=add_fifo_to_list(temp_fifo_list, fifo_port, fifo_path,LISTEN);
				break;
			case LPUBLISH: //cas d'une ligne decrivant un port de sortie
				if (sscanf(buffer, "  publish: %s -> %s", fifo_port, fifo_path)!=2){
					cont = 0;
					break;
				}
				temp_fifo_list = add_fifo_to_list(temp_fifo_list, fifo_port, fifo_path, PUBLISH);
				break;
			case BINARY: //cas d'une ligne decrivant le chemin d'un executable
				//la fonction access verifie que le fichier existe et que l'on a les droits d'execution
				if (sscanf(buffer, "  bin: %s", bin) != 1 || access(bin, X_OK) != 0){
					cont = 0;
					break;
				}
				binary_ok++;
					break;
			case INVALID:
			default:
				fprintf(stderr, "Erreur: Le fichier %s est invalide \n\tVerifiez la ligne n°%d: \e[31m%s\e[0m",path_name,ln_num,buffer);
				cont = 0;
				break;
			}
		} while (cont);
		//Seul le nom et le fichier binaire sont essentiels
		if (name_ok && binary_ok){
			nd_list->nd_list[i] = initialiser_noeud(name, bin, args, temp_fifo_list); //Cree la structure noeud
			nd_list->nb_node++; //incremente le nombre de noeud
		}
		else {
			break;
		}
	}

	//Verifie que tout les noeuds sont lu correctement
	if (nd_list->nb_node < nb_node){
		fprintf(stderr,"Erreur irrecuperable lors de la lecture du fichier\n");
		detruire_fifo(temp_fifo_list);
		detruire_liste_noeud(nd_list);
		fclose(fd);
		return 0;
	}
	fclose(fd);
	return nd_list->nb_node;
}
char** generate_arg_list(node* noeud){
	int i = 0,j;
	fifo* fifo_list=noeud->fifo_list;
	//Compte le nombre de fifo
	while (fifo_list){
		i++;
		fifo_list = fifo_list->next_fifo;
	}
	char**arg_list = malloc(sizeof(char*)*(noeud->nb_args+2+3*i));	//chaque args demande 1 argument Chaque fifo demande 3 arguments 
																	//(--publish ou --liseten, port_name port_path), et le tableau doit
																	//se finir par NULL et commencer par le nom de l'executabe
	arg_list[0] = noeud->name;
	fifo_list = noeud->fifo_list;
	i = 1;
	while (fifo_list){
		if (fifo_list->type == PUBLISH){
			arg_list[i] = copy_string("--publish");
			i++;
		}
		else{
			arg_list[i] = copy_string("--listen");
			i++;
		}
		arg_list[i] = fifo_list->fifo_port;
		i++;
		arg_list[i] = fifo_list->fifo_path;
		i++;
		fifo_list = fifo_list->next_fifo;
	}
	for (j = 0; j < noeud->nb_args; j++){
		arg_list[i] = noeud->args[j];
		i++;
	}
	arg_list[i] = NULL;
	return arg_list;
}

void launch_node(node* noeud){
	int pid;
	if (!noeud){
		fprintf(stderr, "Noeud invalide \n");
		return;
	}
	fprintf(stdout, "Launching node %s...\n", noeud->name);
	fflush(stdout); //Vide le terminal de sortie autrement les données sont perdu apres l'execv
	pid = fork();	//Divise le programme en deux le pere qui va reccupere le PID et le fils qui va executer le noeud
	if (!pid)
	{
		char ** arg_list = generate_arg_list(noeud);	//Reccupere la liste d'argument mise en forme pour execv
		execv(noeud->bin, (char *const*) arg_list);		//Lance le programme avec les parametres decrit par arg_list
		fprintf(stderr, "Error while launching %s\n\r", noeud->name);	//Le code à partir de cette ne s'execute que si le programme rencontre un probleme à l'execution
		free(arg_list);	
		exit(5);
	}
	noeud->pid = pid;//Reccupere le pid afin de pouvoir tuer le programme ulterieurement
}
void launch_all_node(node_list* list){
	int i;
	for (i = 0; i < list->nb_node; i++){
		launch_node(list->nd_list[i]);
	}
}

void kill_all_node(node_list* list) {
	int i;
	for (i = 0; i < list->nb_node; i++) {
		if (list->nd_list[i]->pid >= 0) {
			kill(list->nd_list[i]->pid, SIGNAL); //Tue le programme fils en lui envoyant un signal predefinie
		}
	}
}
void display_config(node_list* config){
	int i, listen_disp=0,publish_disp=0;
	fifo* fifo_list;
	char argsstring[SIZE];
	if (!config){
		fprintf(stderr, "Impossible d'afficher la configuration pointeur invalide\n");
		return;
	}
	for (i = 0; i < config->nb_node; i++){
		listen_disp = 0, publish_disp = 0;
		list_arg_to_string(config->nd_list[i]->args, config->nd_list[i]->nb_args, argsstring); //Reccupere un string formaté de la liste d'argument
		fprintf(stdout, "* %s[%s %s]\n", config->nd_list[i]->name, config->nd_list[i]->bin, argsstring);
		fifo_list = config->nd_list[i]->fifo_list;
		while (fifo_list){
			if (fifo_list->type == PUBLISH){
				fprintf(stdout, "** publish:\n*** %s to %s\n", fifo_list->fifo_port, fifo_list->fifo_path);
				publish_disp++;
			}
			else{
				fprintf(stdout, "** listen:\n*** %s from %s\n", fifo_list->fifo_port, fifo_list->fifo_path);
				listen_disp++;
			}
			fifo_list=fifo_list->next_fifo;
		}
		if (!publish_disp){
			fprintf(stdout, "** publish:\n***\n");
		}
		if (!listen_disp){
			fprintf(stdout, "** listen:\n***\n");
		}
		fprintf(stdout, "\n");
	}
	fflush(stdout);
}
