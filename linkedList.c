
#include "linkedList.h"



void createList (head_t ** list) {
	 if (*list == NULL) {								
		 *list = (head_t *) malloc(sizeof(head_t));	
		 
		 if (*list == NULL) return;					
		 
		 (*list)->first = NULL;	
		 					
	 }
 }
 
// Necesitamos crear el nodo
node_t * CreateNodo(void * valor, int size, key_t key, time_t date, char type[10], char name[15]){
	
	node_t * aux = NULL;
	aux = (node_t*)malloc(sizeof(node_t));
	aux->size = size;
	aux->key = key;
	aux->next = NULL;
	aux->valor = valor;
	strcpy(aux->type, type);
	strcpy(aux->name, name);
	aux->date = time(NULL);
	

}




void Insert (head_t * list, void * valor) {
	if (list == NULL) return;												
	 
	if (list->first == NULL) {											
		node_t * newNode = (node_t *) malloc(sizeof(node_t));				
		list->first = newNode;											
		newNode->next = NULL;												
		newNode->valor = (char *) malloc(sizeof(char)*(strlen(valor)+1));	
		strcpy(newNode->valor, valor);	
		return;																																
	}
	 
	node_t * lastNode = list->first;
	 
	while (lastNode->next != NULL)										
		lastNode = lastNode->next;
	
	node_t * newNode = (node_t *) malloc(sizeof(node_t));				
	lastNode->next = newNode;											
	newNode->next = NULL;												
	newNode->valor = (char *) malloc(sizeof(char)*(strlen(valor)+1));		
	strcpy(newNode->valor, valor);									
 }


 void printList (head_t * list, int items) {
	 if (list->first == NULL) return;						
	 node_t * lastNode = list->first;
	 int index = 1;
	 
	 printf("\n");

	 while (lastNode != NULL && index < items + 1) {			
		 printf ("%d) %s", index, lastNode->valor);			
		 lastNode = lastNode->next;							
		 index++;											
	 }
	 printf("\n");
 }

  char * getNelement (head_t * list, int element) {
	 if (list->first == NULL) return NULL;					
	 
	 node_t * lastNode = list->first;
	 int index = 0;
	 
	 while (lastNode != NULL && index < element) {			
		 lastNode = lastNode->next;								
		 index++;											
	 }
	 
	 if (lastNode != NULL)										 
		return lastNode->valor;											
	 else
	 	return NULL;
										
 }
 
 
  void clearList (head_t * list) {
	 if (list == NULL) return;						
	 
	 node_t * lastNode = list->first;				
	 node_t * tmp = NULL;							
	 
	 while (lastNode->next != NULL) {				
		 tmp = lastNode->next;						
		 lastNode->next = lastNode->next->next;		
		 free(tmp->valor);							
		 free(tmp);									
	 }
	 
	 if (lastNode != NULL) {						
		 free(lastNode->valor);
		 free(lastNode);
	 }
	 list->first = NULL;							
 }

  void removeList (head_t ** list) {
	 if (*list == NULL) return;						
	 
	 node_t * lastNode = (*list)->first;
	 node_t * tmp = NULL;
	 
	 while (lastNode->next != NULL) {					
		 tmp = lastNode->next;							
		 lastNode->next = lastNode->next->next;			
		 free(tmp->valor);								
		 free(tmp);										
	 }
	 
	 if (lastNode != NULL){							
		 free(lastNode->valor);
		 free(lastNode);				
	 }
	 free(*list);							
}


