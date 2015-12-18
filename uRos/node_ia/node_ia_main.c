#include "../common_file/node.h"
#include "../common_file/signal_function.h"


void get_bumper(int fd_br, int fd_bl, char* br, char* bl) {
	char buff[SSIZE_MAX];
	int last_data;
	last_data =read(fd_br, buff, 1);
	*br = buff[last_data-1]; //Lit la derniere valeur presente dans le buffer
	last_data = read(fd_bl, buff, 1);
	*bl = buff[last_data-1]; //Lit la derniere valeur presente dans le buffer
}

void calc_speed(const char br, const char bl, float *vr, float *vl) {
	if (br == '1' || bl == '1')
	{
		*vr = -2;
		*vl = -1;
	}
	else {
		*vr = 1;
		*vl = 1.2;
	}
}

void send_speed(const int fd_com, float vr,float vl) {
	char buff[255];
	sprintf(buff, "set_v %f %f\n", vr, vl);
	write(fd_com, buff, strlen(buff));
}

void read_arg_ia(int argc, char** argv,param* parametre) {
	if (!parametre) {
		fprintf(stderr, "Error *param is empty\n");
		return;
	}

	parametre->set = 0;
	int listen_ok = 0, publish_ok = 0;
	int i;

	// Recherche des différents arguments
	for (i = 1; i < argc; i++) {

		// Argument --listen
		if (!strcmp("--listen", argv[i]) &&!(listen_ok ==11)) {

			if (!strcmp(parametre->listen[0].port, argv[i + 1]) && !(listen_ok / 10)) {
				strcpy(parametre->listen[0].fifo, argv[i + 2]);
				listen_ok += 10;
				i += 2;
			}
			else if (!strcmp(parametre->listen[1].port, argv[i + 1]) && !(listen_ok % 2)) {
				strcpy(parametre->listen[1].fifo, argv[i + 2]);
				listen_ok += 1;
				i += 2;
			}
			else {
				display_usage(5);
			}
		}
		//Argument --publish
		else if (!strcmp("--publish", argv[i]) && !publish_ok) {
			// Port de republication
			if (i + 2 < argc) {
				strcpy(parametre->publish[0].port, argv[i + 1]);
				strcpy(parametre->publish[0].fifo, argv[i + 2]);
				publish_ok++;
				i += 2;
			}
			else {
				display_usage(5);
			}
		}
		else {
			display_usage(5);
			return;
		}
	}

	// Verification du nombre d'argument initialisé
	if (listen_ok==11 && publish_ok)
		parametre->set = 1;
	else
		display_usage(1);
}

int main(int argc,char** argv) {
	sigset_t signals1, signals2; //Variables permettant de gerer les signaux
	float vr, vl;
	int fd_com, fd_br, fd_bl;
	char bumper_r, bumper_l;
	param parametre;
	strcpy(parametre.listen[0].port , "bumper_r");
	strcpy(parametre.listen[1].port , "bumper_l");
	strcpy(parametre.publish[0].port , "?");
	strcpy(parametre.publish[1].port , "0");
	read_arg_ia(argc, argv, &parametre);
	if (parametre.set) {
		disable_signal(&signals1,SIGNALSTOP);
		fd_com = open_fifo(parametre.publish[0].fifo);
		fd_br = read_fifo(&parametre.listen[0]);
		fd_bl = read_fifo(&parametre.listen[1]);
		while (!is_signal_trigered(&signals2, SIGNALSTOP)) {
			get_bumper(fd_br, fd_bl, &bumper_r, &bumper_l);
			calc_speed(bumper_r, bumper_l, &vr, &vl);
			send_speed(fd_com, vr, vl);
			sleep(1);
		}
	}
	else {
		fprintf(stderr,"Erreur durant la lecture des arguments\n");
		return -1;
	}
	close(fd_com);
	close(fd_br);
	close(fd_bl);
	unlink(parametre.publish[0].fifo);
	unlink(parametre.listen[0].fifo);
	unlink(parametre.listen[1].fifo);
	reenable_signal(&signals1);
	return 0;
}