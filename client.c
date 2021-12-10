#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<pthread.h>

#define BUF_SIZE 512	// Taille en octet du buffer d'émission et de réception

void *sendMessage(void *arg);
void *recvMessage(void *arg);

int main(int argc, char *argv[]){

	if(argc < 4){	// On vérifie que les infos de connections ont bien été passées en paramètres
		printf("SYNTAX: %s [ipaddress] [portno] [pseudo]\n", argv[0]);
		exit(1);
	}
	
	int sockfd, portno, retval;		
	struct sockaddr_in server_addr;				// Création de la structure pour gérer la socket
	char buffer[BUF_SIZE];						
	portno = atoi(argv[2]);						// Récupération du numéro de port 
	char* pseudo = malloc(sizeof(char)*50);
	strcpy(pseudo, argv[3]);					// Récupération du pseudo
	printf("\nBonjour %s !\n", pseudo);

// 1) Socket function
	sockfd = socket(AF_INET, SOCK_STREAM, 0);	// Création de la socket
	if(sockfd < 0){
		perror("Socket creation Error");
		exit(1);
	}

	bzero((char *)&server_addr, sizeof(server_addr));	//Fonction de remise à zéro du buffer

	// configuration des données de la structure
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);	//
	if(server_addr.sin_addr.s_addr == -1){
		perror("Inet conversion Error");
		exit(1);
	}
	server_addr.sin_port = htons(portno);		// 

	// Fonction qui gère la connection au serveur à partir de la socket
	if(connect(sockfd, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0){ 	// Vérification de la connexion
		perror("Connection Error");
		exit(1);
	}
	else{
		printf("\n[+] %s s'est connecté au serveur !\n", pseudo);
	}

	bzero(buffer, BUF_SIZE);
	if(read(sockfd, buffer, BUF_SIZE)< 0){			// Lecture des données envoyées sur la socket
 		perror("\nError in sending client id");
 		exit(1);
 	}
	printf("[+] Vos messages seront envoyés au serveur.\n[+] Si vous n'êtes connécté à aucun client, vous recevrez votre message comme réponse du serveur!\n\n");


	bzero(buffer, BUF_SIZE);						// Remise à z&ro du buffer
	retval = write(sockfd, pseudo, strlen(pseudo));	// Ecriture du pseudo sur la socket
	if(retval < 0){									// Analyse des erreurs
		perror("Error in writing pseudo");
		exit(1);
	}

	pthread_t pid1, pid2;	// Création de deux Threads pour envoyer et recevoir les données
	pthread_create(&pid1, NULL, &sendMessage, &sockfd); // Thread de réception des messages
	pthread_create(&pid2, NULL, &recvMessage, &sockfd); // Thread d'émission des messages
	pthread_join(pid1, NULL);
	pthread_join(pid2, NULL);
	close(sockfd);		// Fermeture de la socket
	return 0;
}

void *sendMessage(void *arg){	// Fonction d'envoi des messages sur la socket
	int sockfd = *((int*)arg);
	int retval;					
	char buffer[BUF_SIZE];		// Buffer de récupération des données

	while(1){
		bzero(buffer, BUF_SIZE);		// Remise à zéro du buffer
		fgets(buffer, BUF_SIZE, stdin); // Récupère le message passé dans le terminal et l'écrit dans le buffer

		retval = write(sockfd, buffer, strlen(buffer));	// Ecriture sur la socket 
		if(retval < 0){									// Vérification des erreurs
			perror("Writing Error");
			exit(1);
		}
		if(strncmp(buffer, "Exit", 4) == 0)				// Si le message envoyé est exit, le client se termine
			break;
	}
	return NULL;
}

void *recvMessage(void *arg){		// Fonction de réception des messages
	int sockfd = *((int*)arg);
	char buffer[BUF_SIZE];			// Buffer de réception
	int retval;						// variable de gestion des erreurs

	while(1){						// Boucle infini pour la réception des erreurs
		bzero(buffer, BUF_SIZE);	
		retval = read(sockfd, buffer, BUF_SIZE);	// Lecture des données envoyées sur la socket
		if(retval < 0){
			perror("Reading Error");
			exit(1);
		}

		if(strncmp(buffer, "exit", 4) == 0 || strlen(buffer)<1){	// On vérifie si le client a demandé la fermeture de la connexion
			printf("> Tchat: %s\n", buffer);						// Affichage du message envoyé par le serveur
			break;
		}

		printf("\n> Tchat: %s", buffer);
	}
	return NULL;
}