Auteur:	COLIN Wilson
		MAZOUZ Yanis

Description :	Le but de ce projet est de concevoir et implémenter un middleware pour la robotique.
				A la maniere de ROS ce programme permet de lancer des sous-programmes appeler noeud
				Et de les faires communiquer entre eux.

Dependences	:	Ce programme necessite que les librairies suivante soit installer sur le systeme:
				SDL1.2, fastsim, boost 

Compilation :	Pour compiler le projet il suffit de lancer make à la racine, le master ainsi que	
				l'ensemble des noeuds sont compilés et disponible dans le dossier exe

Master		:	Master permet de lire un fichier de configuration, de tester sa validité, lancer les
				les executables avec les bons arguments et de s'assurer de leur fermeture à la fin  
				de l'execution.
				Pour se faire le master crée des processus fils dont le PID est enregistré, et lors-
				que la condition d'arrete (SIGINT: Ctrl-C) est reçu, master envoie un signal d'arret
				aux differents processus afin d'entamer le processus d'extinction.
				./master config.txt
				config.txt un fichier de configuration formaté definissant les executables à lancer
				et les parametres associés.

Node_echo	:	Node echo permet de broadcaster un message predefinie à interval reguliere sur un
				port specifique.
				./node_echo --publish out_port myfifo.fifo --msg MSG --interval time
				myfifo.fifo indique l'adresse du port sur lequel broadcaster
				MSG indique le message à transmettre
				time l'intervale de temps entre deux messages.

Node_ia		:	Node ia permet de controler le robot de maniere autonome, une fois lancer le progra-
				mme va se servir des informations provenent des bumpers pour controler le robot, da-
				ns l'etat actuel du programme à un fonctionnement sensorimoteur basé sur les bumpers
				./node_ia --publish command command.fifo --listen bumper_l bumperl.fifo --listen 
				bumper_r bumperr.fifo
				command.fifo port sur lequel les commandes sont envoyées
				bumperl.fifo bumperr port sur lequel sont lus les etats de bumpers

Node_simu	:	Node simu permet de simuler un robot et l'afficher à l'ecran, le noeud lit un port
				de commande servant à definir les vitesses du robot et publie l'etat des bumpers su-
				r un port de sortie. Le programme est constitué d'une boucle principale permettant 
				de gerer la gestion de l'affichage et de deux threads l'un dedier à la lecture du po
				-rt de commande et l'autre à la publication de l'etat des bumpers. Si la commande
				kill est reçu sur le port de commande ou qu'un SIGINT est reçu le programme interrom
				-pe sont executions et se ferme.
				./node_simu --commande myfifo.fifo --map <carte.pbm> --init-pos <x_initial>
				 <y_initial> --publish bumper_l bl.fifo bumper_r br.fifo

Node_tele	:	Node simu permet de lire les commandes saisie par l'utilisateur, de verifier leur co
				formités sythaxique et d'envoyer la commande sur le port de sortie en cas de command
				-de valide. La commande kill est transmise avant de provoquer l'extinction du proces
				-sus
				./node_tele --publish out output.fifo

Node_logger	:	Node logger permet d'enregistrer le flux d'un port dans un fichier texte et d'eventu
				-ellement le retransmettre sur un autre port. Le programme reagit au signaux SIGINT
				en arretant le logging et en sauvegardant le fichier sur le disque avant de s'eteind
				-re
				./node_logger --listen to_log port-ecoute.fifo -- publish copy_out port-recopie.fifo 
				--logfile log.txt> 

--------------------------------Testes validation---------------------------------------------------

Node_logger et Node_echo	: ./master config_simple.txt, permet de tester le fonctionnement des de-
				ux noeuds.Node_echo envoie un message toute les secondes que node_logger enregistre
				dans le ficihier log.txt. Pour arreter proprement le programme il suffit d'envoyer
				un SIGINT en appuyant sur ctrl-C

Node_tele et Node_simu	: ./master config_robot.txt permet de tester le fonctionnement des de-
				ux noeuds.Node_tele demande à l'utilisateur de saisir des commandes, verifie leur
				validité avant de les envoyer, vers node_simu Pour arreter proprement le programme 
				il faut saisir la commande kill

Node_ia et Node_simu	: ./master config_robot_ia.txt permet de tester le fonctionnement des de-
				ux noeuds. Node_ia reccupere les informations publié par node simu, les traites et
				renvoie des commande de vitesse en consequence, ctrl-C provoque l'arrete du program

Master					: ./master config_robot_corrupted.txt permet de verifier le comportement du
				master avec un fichier invalide
