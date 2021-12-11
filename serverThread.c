#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<pthread.h>
#include "linkedlist.h"

#define BUF_SIZE 512
#define NO_OF_CLIENTS 10
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
int clientfd[NO_OF_CLIENTS];

void* socketChat(void *arg){
	char buffer[BUF_SIZE];		// Buffer de réception des données
	char temp[BUF_SIZE];		//
	int retval;					// Variable de gestion des erreurs
	int clientfd = *((int*)arg);
	char* pseudo = malloc(sizeof(char)*50);	// Pseudo du client
 	int targetclient=-1;					// Client cible
 	int result = clientfd;
 	char a = clientfd + '0';				

 	retval = write(clientfd, &a, 1);			// Envoi de son id au client 
 	if(retval < 0){
 		perror("\nError in sending client id");
 		exit(1);
 	}

	bzero(buffer, BUF_SIZE);
	retval = read(clientfd, buffer, 50);	// Réception du pseudo
	strcpy(pseudo, buffer);					// copie du pseudo depuis le buffer
	printf("%s\n", pseudo);
 	if(retval < 0){
 		perror("\nError in receiving pseudo");
 		exit(1);
	}

	int id = result;
	setPseudo(id, pseudo);					// Ecriture du pseudo dans la LinkedList

	while(1){				
		bzero(buffer, BUF_SIZE);
		retval = read(clientfd, buffer, BUF_SIZE);	// Lecture du message et écriture dans clientfd

		if(retval < 0){
			perror("\nReading Error\n");
			break;
		}
		printf("Client %d (%s): %s", clientfd, getPseudoFromID(clientfd), buffer);	// Affichage des infos dans le terminal du serveur
		if(strncmp(buffer, "Exit", 4) == 0 || strlen(buffer) < 1){
			result = clientfd;
			break;
		}
		else if(strncmp(buffer,"Envoi", 5)==0){				// Si le message contient l'instruction Envoi
			char * tmp = extractPseudo(buffer + 6);			// On récupère le pseudo du destinataire
			targetclient = getIDFromPseudo(tmp);			// On récupère l'id du destinataire
			if(isValid(targetclient, clientfd) == 1){		// Si le client est valide (toujours connecté)
				printf("[+] Client %s est connecté au client %s\n", getPseudoFromID(clientfd), getPseudoFromID(targetclient));

				bzero(buffer, BUF_SIZE);					// Remise à zéro du buffer
				strcpy(buffer, "Connexion réussie !\n");	// Copie du message dans le buffer
				result = clientfd;
				fflush(stdin);								// On vide le buffer de l'entrée standard
			}
			else{
				printf("[+] %s ne peut pas se connecter à %s\n", getPseudoFromID(clientfd), getPseudoFromID(targetclient));	// Infos connexion impossible
				bzero(buffer, BUF_SIZE);
				strcpy(buffer, "Client invalide\n");	// Envoi du message d'erreur au client 
				targetclient = -1;
			}
		}
		else if(strncmp(buffer,"Liste",5)==0){		// Si le message contient l'instruction Liste
			bzero(buffer, BUF_SIZE);
			strcpy(buffer, print(clientfd));		// Affichage de tous les clients connectés via la LinkedList
			printf("\n%s", print());
			result = clientfd;
			fflush(stdin);
		}
		else if(strncmp(buffer,"Quit",4)==0){		// Si le message contient l'instruction Quit
			printf("\n[+] %s s'est déconnecté de %s\n", getPseudoFromID(clientfd), getPseudoFromID(targetclient));
			bzero(buffer, BUF_SIZE);
			result = targetclient;

			strcpy(buffer, strcat(getPseudoFromID(clientfd), " s'est déconnecté !\n"));	// Copie du message d'information de déconnexion dans le buffer

			retval = write(result, buffer, strlen(buffer)); // Envoi de l'information de déconnexion au client 
			if(retval < 0){
				perror("Writing Error");
				break;
			}

			bzero(buffer, BUF_SIZE);
			strcpy(buffer, "Déconnexion réussie !\n");
			targetclient=-1;
			result = clientfd;
			fflush(stdin);

		}
		else{
			result = targetclient;
			if(result!=-1)
			{
				// Changement du format du message
				// Msg du client CLIENTFD : msg
				bzero(temp, BUF_SIZE);
				strcpy(temp, buffer);
				bzero(buffer, BUF_SIZE);
				strcpy(buffer, "Msg de ");
				strcat(buffer, getPseudoFromID(clientfd));
				strcat(buffer, ": ");
				strcat(buffer, temp);

				printf("\n[+] Envoi d'un message de %s à %s\n", getPseudoFromID(clientfd), getPseudoFromID(result));
				// Envoi du message au client cible
				bzero(temp, BUF_SIZE);
				strcpy(temp, "Envoi du message à ");
				strcat(temp, getPseudoFromID(targetclient));
				strcat(temp, "\n");
				retval = write(clientfd, temp, sizeof(temp));
				if(retval < 0){
					perror("Writing Error");
					break;
				}
			}
		}

		if(result == -1){
			result = clientfd;	// Le message sera renvoyé à l'émetteur
		}

		fflush(stdin);
		retval = write(result, buffer, strlen(buffer));	// Ecriture du message sur la socket
		if(retval < 0){
			perror("Writing Error");
			break;
		}
	}
	pthread_mutex_lock(&lock);		// Blocage des accès concurents pour la suppression d'un client dans la LinkedList
	findandremove(clientfd);		// Suppresion du client
	pthread_mutex_unlock(&lock);
	retval = write(result, buffer, strlen(buffer)); //send exit message to client
	if(retval < 0){
		perror("Writing Error");
	}
	printf("\n[+] Client %d exiting\n", clientfd);
}


int main(int argc, char *argv[]){

	if(argc < 2){
		printf("SYNTAX: %s [portno]\n", argv[0]);
		exit(1);
	}

	// Configuration

	int sockfd, clientfd, portno, retval;
	struct sockaddr_in server_addr, client_addr;
	socklen_t clientlen;
	portno = atoi(argv[1]);		// Ecriture du numéro de port d'écoute

// 1) Socket function
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0){
		perror("Socket creation Error");
		exit(1);
	}
	bzero((char *)&server_addr, sizeof(server_addr)); // Remplissage avec des zeros
	// set the values in structure
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(portno);

// 2) Bind
	if(bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
		perror("Binding Error");
		exit(1);
	}

// 3) Listen
	if(listen(sockfd, NO_OF_CLIENTS) == 0)
		printf("Listening on port: %d", portno);
	else
		printf("Error in listening");

	pthread_t pid[NO_OF_CLIENTS];
	int i = 0;
	while(1 && i < NO_OF_CLIENTS){
	// 4) Accept
		clientlen = sizeof(client_addr);
		clientfd = accept(sockfd, (struct sockaddr*)&client_addr, &clientlen);
		if(clientfd < 0){
			perror("Accepting Error");
			exit(1);
		}
		else{
			pthread_mutex_lock(&lock);	// Prise du lock pour éviter les modifications concurentes pendant la création du client
			create(clientfd);
			pthread_mutex_unlock(&lock); 
			printf("\n[+] Client %d s'est connecté au serveur\n", clientfd);
		}
	// 5) Parallélisme pour gérer les différents chat
		pthread_create(&pid[i],NULL,&socketChat,&clientfd);
		i++;
	}
	close(sockfd);	// Fermeture de la socket

	return 0;
}
