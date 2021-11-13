#ifndef linkedlist_h
#define linkedlist_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct node {
    void * valor;
    int size;
    key_t key;
    time_t date;
    char type[10]; // Command, malloc, mmap, shared
    char file[20];
    int fd;
    struct node * next;
}node_t;



typedef struct head {
	node_t * first;
}head_t;

void printList (head_t * list, int items, char type[20]);

#endif