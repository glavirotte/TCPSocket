#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>

// Notre LinkedList nous permet de gérer dynamiquement l'ajout ou la suppression des clients 

struct Llist{
    int client_id;        // Identifiant utilisé par le serveur pour désigner le client c
    char* client_pseudo;  // Pseudo du client
    struct Llist *next;   // Pointeur sur le client suivant
};

struct Llist *head = NULL,*tail = NULL; // Déclaration de la LinkedList


void create(int c_id){

 struct Llist *nn = (struct Llist*)malloc(sizeof(struct Llist));  // Création de la LinkedList allouée dynamiquement

 if(nn == NULL){
  printf("Error in allocating memory!");
  return;
 }

 nn->client_id = c_id;
 nn->client_pseudo = NULL;
 
 if(head == NULL && tail == NULL){
  head = nn; 
  tail = nn;
  nn->next = NULL;
 }else{
  tail->next = nn;
  tail = nn;
  tail->next = NULL;
 }
}

void setPseudo(int c_id, char* c_pseudo){   // Fonction qui écrit le pseudo du client c 
 
 if(head == NULL){
  printf("Linked list is empty!");
  return;
 }

 struct Llist *ptr = head;
 struct Llist *prevptr = NULL;
 
 while(ptr != NULL){              // Parcourt de la liste jusqu'à trouver le client
  if(ptr->client_id == c_id){
    ptr->client_pseudo = c_pseudo;
    return;
  }

  prevptr = ptr;
  ptr = ptr->next;
 }
}

void findandremove(int c_id){   // Suppression du client c
 
 if(head == NULL){
  printf("Linked list is empty!");
  return;
 }

 struct Llist *ptr = head;
 struct Llist *prevptr = NULL;
 
 while(ptr != NULL){          // Parcourt de la liste jusqu'à trouver le client recherché
  if(ptr->client_id == c_id){
    
   if(prevptr == NULL){
     head = head->next;
     if(head == NULL)
     	tail = NULL;
     free(ptr);               // Désallocation du client
     break;
   }else{
     prevptr->next = ptr->next;
     free(ptr);
     break;
   }
  }

  prevptr = ptr;
  ptr = ptr->next;
 }
}

char* print(int c){       // Fonction qui affiche le client voulu
 
 struct Llist *ptr = head;
 int i = 1;
 int online = 0;
 char *result=(char*)malloc(500 * sizeof(char));
 strcat(result, "Clients en ligne:\n");
 while(ptr != NULL){
   if(ptr->client_id != c){
	strcat(result, "[*] ");
  strcat(result, ptr->client_pseudo);
  strcat(result,"\n");
	online++;
   }
   ptr = ptr->next;
   }
 if(online == 0)
	strcpy(result, "Aucun client en ligne\n");
   return result;
}

int isValid(int c, int clientfd){   // Vérification de la validité d'un client dans la liste
	struct Llist *ptr = head;
	while(ptr != NULL){
	   if(ptr->client_id == c && ptr->client_id != clientfd){
		return 1;
	   }
	 	ptr = ptr->next;  
	}
	return 0;
}

char* getPseudoFromID(int c_id){    // Retourne le pseudo d'un client à partir de son identifiant
 
 if(head == NULL){
  printf("Linked list is empty!");
  return NULL;
 }

 struct Llist *ptr = head;
 struct Llist *prevptr = NULL;
 
 while(ptr != NULL){
  if(ptr->client_id == c_id){
    return ptr->client_pseudo;
  }else{
    prevptr = ptr;
    ptr = ptr->next;
    }
  }

}

int getIDFromPseudo(char * pseudo){   // Retourne l'identifiant du client à partir de son pseudo
 
 if(head == NULL){
  printf("Linked list is empty!");
  return -1;
 }

 struct Llist *ptr = head;
 struct Llist *prevptr = NULL;
 
 while(ptr != NULL){
  if(strcmp(ptr->client_pseudo,pseudo) == 0){
    return ptr->client_id;
  }else{
    prevptr = ptr;
    ptr = ptr->next;
    }
  }

}

// Fonction complémentaire pour extraire le pseudo du reste des données (pas de rapport avec la LinkedList)

char* extractPseudo(char* buffer){
  char * pseudo = malloc(sizeof(char)*50);
  char c = buffer[0];
  int i = 0;

  while(c != '\n'){
    *(pseudo+i) = *(buffer+i);
    i++;
    c = buffer[i];
  }
  return pseudo;

}