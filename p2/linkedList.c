
#include "linkedList.h"
#include <time.h>


void createList (head_t ** list) {
	 if (*list == NULL) {								
		 *list = (head_t *) malloc(sizeof(head_t));	
		 
		 if (*list == NULL) return;					
		 
		 (*list)->first = NULL;	
		 					
	 }
 }
 

void Insert(head_t * list, void * valor, int size, key_t key, time_t date, char type[10]) {
	if (list == NULL) return;												
	 
	if (list->first == NULL) {											
		node_t * newNode = (node_t *) malloc(sizeof(node_t));				
		list->first = newNode;											
		newNode->next = NULL;
		newNode->size = size;
		strcpy(newNode->type, type);
		newNode->key = key;	
		if (strcmp(type, "historial") == 0) strcpy(newNode->valor, valor);
		else newNode->valor = valor;	
										
		newNode->date = time(NULL);

		return;																																
	}
	node_t * lastNode = list->first;
	 
	while (lastNode->next != NULL){									
		lastNode = lastNode->next;
	}
	node_t * newNode = (node_t *) malloc(sizeof(node_t));				
	newNode->next = NULL;
	newNode->size = size;
	newNode->key = key;	
	strcpy(newNode->type, type);
	newNode->date = time(NULL);											
	newNode->valor = valor;	
	lastNode->next = newNode;																		
}

void printList (head_t * list, int items) {
	 if (list->first == NULL) {
	 	printf("\nEmpty list\n\n");
		return;
	 }
	 node_t * lastNode = list->first;
	 int index = 1;
	 
	 printf("\n");
	
	 while (lastNode != NULL && index < items + 1) {			
		if(strcmp(lastNode->type, "historial") == 0){
			printf ("%d) %s", index, (char *)lastNode->valor);			
		}
		if(strcmp(lastNode->type, "malloc") == 0){
			char date[25];
			struct tm * tm = localtime(&(lastNode->date));
			strftime(date, 100, "%c", tm);
			printf("%p: size:%d. malloc  %s\n", lastNode->valor, lastNode->size, date);
		}
		
	lastNode = lastNode->next;							
	index++;
	}	
	printf("\n");									
}




void deleteMemory(head_t * list, int size){
	if (list == NULL){
		printf("No blocks of memory allocated\n");
		return;
	} 

	else{
		node_t * after = list->first;
		node_t * before = list->first;

		if (before->size == size){
			if (strcmp(before->type, "malloc") == 0){
				printf("deallocated %d at %p\n", size, before->valor);
				before = before->next;
				free(before->valor);					
				free(before);
				return;
			}
		}
		else{
			after = after->next;
			while(after != NULL){
				if (after->size == size){
					if (after->next == NULL){
						if (strcmp(after->type, "malloc") == 0){
						printf("deallocated %d at %p\n", size, after->valor);
						before->next = NULL;
						free(after->valor);					
						free(after);
						return;
						}
					}
					else{
						if (strcmp(after->type, "malloc") == 0){
						before->next = after->next;
						printf("deallocated %d at %p\n", size, after->valor);
						free(after->valor);					
						free(after);
						return;
						}	
					}
				}
			after = after->next;
			before = before->next;
			}
			printf("No such block exists, printing the list of addresses alocated...\n");
			printList(list, INT_MAX);
		}
	}
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


