#ifndef LL_H
#define LL_H

void create(int c_id);
void setPseudo(int c_id, char* c_pseudo);
void findandremove(int c_id);
char* print();
int isValid(int c, int clientfd);
char* getPseudoFromID(int c_id);
int getIDFromPseudo(char * pseudo);
char* extractPseudo(char* buffer);
char* extractFirstMessage(char* buffer);

#endif
