
#include "linkedList.h"
#include <time.h>
#include <limits.h>
#include <sys/mman.h>


void createList (head_t ** list) {
	 if (*list == NULL) {								
		 *list = (head_t *) malloc(sizeof(head_t));	
		 
		 if (*list == NULL) return;					
		 
		 (*list)->first = NULL;	
		 					
	 }
 }
 

void Insert(head_t * list, void * valor, int size, key_t key, time_t date, char type[15], char file[20], int fd) {
	if (list == NULL) return;												
	 
	if (list->first == NULL) {											
		node_t * newNode = (node_t *) malloc(sizeof(node_t));				
		list->first = newNode;											
		newNode->next = NULL;
		newNode->size = size;
		strcpy(newNode->type, type);
		strcpy(newNode->file, file);
		newNode->fd = fd;
		newNode->key = key;	
		newNode->date = time(NULL);
		newNode->valor=strdup(valor);							
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
	strcpy(newNode->file, file);
	newNode->fd = fd;
	newNode->date = time(NULL);												
	newNode->valor=strdup(valor);	
	lastNode->next = newNode;																		
}

void printList (head_t * list, int items, char type[20]) {
	if (list->first == NULL) {
		printf("\nEmpty list\n\n");
		return;
	}
	node_t * lastNode = list->first;
	int index = 1;
	 
	printf("\n");
	
	while (lastNode != NULL && index < items + 1) {			
		
		if(strcmp(type, "historial") == 0){
			if (strcmp(lastNode->type, "historial") == 0){
				printf ("%d) %s", index, (char *)lastNode->valor);			
			}
		}
		if(strcmp(lastNode->type, "malloc") == 0 && strcmp(type, "malloc") == 0 || (strcmp(type, "MALL") == 0 && strcmp(lastNode->type, "malloc") == 0)){
			char date[25];
			struct tm * tm = localtime(&(lastNode->date));
			strftime(date, 100, "%c", tm);
			printf("%p: size:%d. malloc  %s\n", lastNode->valor, lastNode->size, date);
		}
		else if(strcmp(lastNode->type, "mmap") == 0 && strcmp(type, "mmap") == 0 || (strcmp(type, "MALL") == 0 && (strcmp(lastNode->type, "mmap") == 0))){
			char date[25];
			struct tm * tm = localtime(&(lastNode->date));
			strftime(date, 100, "%c", tm);
			printf("%p: size:%d. mmap %s (fd:%d) %s\n", lastNode->valor, lastNode->size, lastNode->file, lastNode->fd, date);
		}
		else if (strcmp(lastNode->type, "shared") == 0 && strcmp(type, "shared") == 0 || (strcmp(type, "MALL") == 0 && (strcmp(lastNode->type, "shared") == 0))){
			char date[25];
			struct tm * tm = localtime(&(lastNode->date));
			strftime(date, 100, "%c", tm);
			printf("%p: size:%d. key:%d. shared %s\n", lastNode->valor, lastNode->size, lastNode->key, date);
		}
	lastNode = lastNode->next;							
	index++;
	}	
	printf("\n");									
}

void deleteNode (node_t ** node) {
	if(strcmp((*node)->type, "malloc") == 0) {
		free((*node)->valor);
	} else if(strcmp((*node)->type, "shared") == 0) {
		shmdt((*node)->valor);
	}
	free(*node);
}

void deleteMemoryAtKey(head_t * list, key_t key) {
	if(list!=NULL) {
		node_t * aux = list->first;
		node_t * prev = list->first;
		
		if(aux->key == key) {
			list->first = aux->next;
			printf("adress %p deallocated (%s)\n", aux->valor, aux->type);
			shmdt(aux->valor);
			free(aux);
			return;
		} else {
			aux = aux->next;
			while(aux!=NULL) {
				if(aux->key == key && strcmp(aux->type, "shared") == 0) {
					if(aux->next == NULL){
						prev->next = NULL;
						printf("address %p deallocated (%s)\n", aux->valor, aux->type);
						shmdt(aux->valor);
						free(aux);
						return;
					} else {
						if (strcmp(aux->type, "shared") == 0){
						prev->next = aux->next;
						printf("address %p deallocated (%s)\n", aux->valor, aux->type);
						shmdt(aux->valor);
						free(aux);
						return;
						}						
					}
				}
				aux = aux->next;
				prev = prev->next;
			}
			printf("key %d does not exist\n", key);
			return;
		}
	}
	printf("No memory reserved\n");
}

void deleteMemoryAtSize(head_t * list, int size){
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
				list->first = before->next;
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
						free(after->valor);					
						free(after);
						before->next = NULL;
						return;
						}
					}
					else{
						if (strcmp(after->type, "malloc") == 0){
						printf("deallocated %d at %p\n", size, after->valor);
						free(after->valor);					
						free(after);
						before->next = after->next;
						return;
						}	
					}
				}
			after = after->next;
			before = before->next;
			}
			printf("No such block exists, printing the list of addresses alocated...\n");
			printList(list, INT_MAX, "malloc");
		}
	}
}

void deleteMemoryAtFilename(head_t * list, char file[20]){
	if (list == NULL){
		printf("No blocks of memory allocated\n");
		return;
	} 

	else{
		node_t * after = list->first;
		node_t * before = list->first;

		if (strcmp(before->file, file) == 0){
			if (strcmp(before->type, "mmap") == 0){
				printf("File %s unmapped at %p\n", before->file, before->valor);
				munmap(before->valor, before->size);		
				free(before);			
				list->first = before->next;
				return;
			}
		}
		else{
			after = after->next;
			while(after != NULL){
				if (strcmp(after->file, file) == 0){
					if (after->next == NULL){
						if (strcmp(after->type, "mmap") == 0){
						printf("File %s unmapped at %p\n", after->file, after->valor);
						munmap(after->valor, after->size);
						free(after);
						before->next = NULL;
						return;
						}
					}
					else{
						if (strcmp(after->type, "mmap") == 0){
						printf("File %s unmapped at %p\n", after->file, after->valor);
						munmap(after->valor, after->size);
						free(after);
						before->next = after->next;
						return;
						}	
					}
				}
			after = after->next;
			before = before->next;
			}
			printf("File is not mapped or not specified, printing the list of files mapped...\n");
			printList(list, INT_MAX, "mmap");
		}
	}
}

void deleteMemoryAtAdress(head_t * list, char address[17]){
	
	void *p;
    
	p=(void *)strtol(address,NULL,16);

	if (list == NULL){
		printf("No blocks of memory allocated\n");
		return;
	} 

	else{
		node_t * after = list->first;
		node_t * before = list->first;

		if (before->valor == p){
			if (strcmp(before->type, "mmap") == 0){
				list->first = before->next;
				printf("File %s unmapped at %p\n", before->file, before->valor);
				munmap(before->valor, before->size);		
				free(before);			
				return;
			}
			else if (strcmp(before->type, "malloc") == 0){
				printf("deallocated %d at %p\n", before->size, before->valor);
				list->first = before->next;
				free(before->valor);					
				free(before);
				return;
			}
		}
		else{
			after = after->next;
			while(after != NULL){
				if (after->valor == p){
					if (after->next == NULL){
						if (strcmp(after->type, "mmap") == 0){
						printf("File %s unmapped at %p\n", after->file, after->valor);
						munmap(after->valor, after->size);
						free(after);
						before->next = NULL;
						return;
						}
						else if(strcmp(after->type, "malloc") == 0){
						printf("deallocated %d at %p\n", after->size, after->valor);
						before->next = NULL;
						free(after->valor);					
						free(after);
						
						return;
						}
					}
					else{
						if (strcmp(after->type, "mmap") == 0){
						printf("File %s unmapped at %p\n", after->file, after->valor);
						munmap(after->valor, after->size);
						free(after);
						before->next = after->next;
						return;
						}	
						if (strcmp(after->type, "malloc") == 0){
						printf("deallocated %d at %p\n", after->size, after->valor);
						before->next = after->next;
						free(after->valor);					
						free(after);
						return;
						}	
					}
				}
			after = after->next;
			before = before->next;
			}
			printf("\nCannot find the introduced memory address, printing the list with al memory management...\n");
			printList(list, INT_MAX, "MALL");
		}
	}
}

void * searchByFilename(head_t * list, int items, char file[20]) {
	 if (list->first == NULL) {
	 	printf("\nEmpty list\n\n");
		return NULL;
	 }

	 node_t * lastNode = list->first;
	 int index = 1;

	while (lastNode != NULL && index < items + 1) {			
		if(strcmp(lastNode->type, "mmap") == 0){
			if (strcmp(file, lastNode->file) == 0){
				return lastNode->valor;
			}	
		}
	lastNode = lastNode->next;							
	index++;
	}
	return NULL;	
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


