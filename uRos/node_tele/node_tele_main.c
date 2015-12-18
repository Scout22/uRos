#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "../common_file/node.h"
#include <signal.h>
#include <sys/types.h>
#define SIGNALSTOP SIGINT

int main(int argc,char** argv) {
	char buffer[SIZE];
	int continu = 1;
	char fifo[SIZE];
	read_arg_tele(argc,argv,fifo);
	if (fifo[0] == '\0') {
		display_usage(4);
		return -1;}
	int fd=open_fifo(fifo);
	
	if (fd > 0) {
		fprintf(stdout, "\033[2JMode teleoperation\n envoyer la commande kill pour arreter le programme\n");
		while (continu) {
			fprintf(stdout, "Saisissez la commande à envoyé\n");
			fgets(buffer, SIZE, stdin);
			switch (commande_type(buffer)) {
			case KILL:
				kill(getppid(), SIGNALSTOP);
				continu = 0;
			case CCW:
			case CW:
			case FORWARD:
			case BACKWARD:
			case SETV:
			case STOP:
				fprintf(stdout, "Commande valide\n");
				write(fd, buffer, strlen(buffer));
				break;
			default:
			case INVALID:
				fprintf(stdout, "Commande invalide\n");
				break;
			}
		}
		close(fd);
	}
	unlink(fifo);
	return 0;
}