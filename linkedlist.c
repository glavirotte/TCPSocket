#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>

struct Llist{
    int client_id;
    char* client_pseudo;
    struct Llist *next;
};

struct Llist *head = NULL,*tail = NULL;


void create(int c_id){

 struct Llist *nn = (struct Llist*)malloc(sizeof(struct Llist));

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

void setPseudo(int c_id, char* c_pseudo){
 
 if(head == NULL){
  printf("Linked list is empty!");
  return;
 }

 struct Llist *ptr = head;
 struct Llist *prevptr = NULL;
 
 while(ptr != NULL){
  if(ptr->client_id == c_id){
    ptr->client_pseudo = c_pseudo;
    return;
  }

  prevptr = ptr;
  ptr = ptr->next;
 }
}

void findandremove(int c_id){
 
 if(head == NULL){
  printf("Linked list is empty!");
  return;
 }

 struct Llist *ptr = head;
 struct Llist *prevptr = NULL;
 
 while(ptr != NULL){
  if(ptr->client_id == c_id){
    
   if(prevptr == NULL){ //if present in head
     head = head->next;
     if(head == NULL)
     	tail = NULL;
     free(ptr);
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

char* print(int c){
 
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

int isValid(int c, int clientfd){
	struct Llist *ptr = head;
	while(ptr != NULL){
	   if(ptr->client_id == c && ptr->client_id != clientfd){
		return 1;
	   }
	 	ptr = ptr->next;  
	}
	return 0;
}

char* getPseudoFromID(int c_id){
 
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

int getIDFromPseudo(char * pseudo){
 
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

// Fonction complémentaire pour extraire le pseudo du reste des données

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