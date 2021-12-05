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
	char buffer[BUF_SIZE];
	char temp[BUF_SIZE];
	int retval;
	int clientfd = *((int*)arg);
	char* pseudo = malloc(sizeof(char)*50);
 	int targetclient=-1;
 	int result = clientfd;
 	char a = clientfd + '0';

 	retval = write(clientfd, &a, 1);
 	if(retval < 0){
 		perror("\nError in sending client id");
 		exit(1);
 	}

	bzero(buffer, BUF_SIZE);
	retval = read(clientfd, buffer, 50);
	strcpy(pseudo, buffer);
	printf("%s\n", pseudo);
 	if(retval < 0){
 		perror("\nError in receiving pseudo");
 		exit(1);
	}

	int id = result;
	setPseudo(id, pseudo);

	while(1){
		bzero(buffer, BUF_SIZE);
		retval = read(clientfd, buffer, BUF_SIZE);

		if(retval < 0){
			perror("\nReading Error\n");
			break;
		}
		printf("Client %d (%s): %s", clientfd, getPseudoFromID(clientfd), buffer);
		if(strncmp(buffer, "exit", 4) == 0 || strlen(buffer) < 1){
			result = clientfd;
			break;
		}
		else if(strncmp(buffer,"Envoi", 5)==0){
			char * tmp = extractPseudo(buffer + 6);
			targetclient = getIDFromPseudo(tmp);
			if(isValid(targetclient, clientfd) == 1){
				printf("[+] Client %s est connecté au client %s\n", getPseudoFromID(clientfd), getPseudoFromID(targetclient));
				bzero(buffer, BUF_SIZE);
				strcpy(buffer, "Connexion réussie !\n");
				result = clientfd;
				fflush(stdin);
			}
			else{
				printf("[+] %s ne peut pas se connecter à %s\n", getPseudoFromID(clientfd), getPseudoFromID(targetclient));
				bzero(buffer, BUF_SIZE);
				strcpy(buffer, "Client invalide\n");
				targetclient = -1;
			}
		}
		else if(strncmp(buffer,"Liste",5)==0){
			bzero(buffer, BUF_SIZE);
			strcpy(buffer, print(clientfd));
			printf("\n%s", print());
			result = clientfd;
			fflush(stdin);
		}
		else if(strncmp(buffer,"quit",4)==0){
			printf("\n[+] %s s'est déconnecté de %s\n", getPseudoFromID(clientfd), getPseudoFromID(targetclient));
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
				// Envoi du message au targetClient
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
			result = clientfd;
		}

		fflush(stdin);
		retval = write(result, buffer, strlen(buffer));
		if(retval < 0){
			perror("Writing Error");
			break;
		}
	}
	pthread_mutex_lock(&lock);
	findandremove(clientfd);
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

	int sockfd, clientfd, portno, retval;
	struct sockaddr_in server_addr, client_addr;
	socklen_t clientlen;
	portno = atoi(argv[1]);

// 1) Socket function
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0){
		perror("Socket creation Error");
		exit(1);
	}
	bzero((char *)&server_addr, sizeof(server_addr)); // fills with zero

	// set the values in structure
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // any local address
	server_addr.sin_port = htons(portno);

// 2) Bind function
	if(bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
		perror("Binding Error");
		exit(1);
	}

// 3) Listen function
	if(listen(sockfd, NO_OF_CLIENTS) == 0)
		printf("Listening on port: %d", portno);
	else
		printf("Error in listening");

	pthread_t pid[NO_OF_CLIENTS];
	int i = 0;
	while(1 && i < NO_OF_CLIENTS){
	// 4) Accept function
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
	// 5) Threading
		pthread_create(&pid[i],NULL,&socketChat,&clientfd);
		i++;
	}
	close(sockfd);

	return 0;
}
