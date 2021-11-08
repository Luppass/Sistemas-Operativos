//Libreria que proporciona todas las funciones para utilizar la lista dinámica del historial de nustras prácticas de SO
//Librerias usadas
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/shm.h>

//Constantes
#define LENGTH 100

//Palabras clave

//Estructuras creadas
typedef struct nodo{
    void * valor;
    char nombre[20];
    char tipo[20];
    int size;
    key_t clave;
    time_t fecha;
    struct nodo * sig;
} nodo;

typedef struct nodo * pos;
#include "historico.h"


//FUNCIONES
//Funcion que crea la lista
nodo * CrearLista(){
    struct nodo * lista = NULL;
    //lista = (struct nodo *) malloc(sizeof(struct nodo));
    return lista;
}

//Funcion que crea un elemento del tipo nodo y le asigna un valor de sig igual a NULL para insertarlo al final
nodo * CrearItem(void * contenido, char nombre[20], char tipo[20], int size, key_t clave){
    struct nodo * auxiliar = NULL;
    auxiliar = (struct nodo *) malloc(sizeof(struct nodo));
    auxiliar->sig = NULL;
    auxiliar->valor = contenido;
    auxiliar->size = size;
    strcpy(auxiliar->tipo, tipo);
    strcpy(auxiliar->nombre, nombre);
    auxiliar->fecha = time(NULL);
    auxiliar->clave = clave;
    return auxiliar;
}

//Funcion que inserta un item al final de la lista
int InsertarItem(nodo ** lista, void * contenido, char nombre[20], char tipo[20], int size, key_t clave){
    if (*lista == NULL){
        *lista = (struct nodo *) malloc(sizeof(struct nodo));
        (*lista)->valor = contenido;
        (*lista)->sig = NULL;
        (*lista)->size = size;
        strcpy((*lista)->tipo, tipo);
        strcpy((*lista)->nombre, nombre);
        (*lista)->fecha = time(NULL);
        (*lista)->clave = clave;
        return 0;
    } else {
        struct nodo * auxiliar = *lista;
        struct nodo * item;
        while (auxiliar->sig != NULL){
            auxiliar = auxiliar->sig;
        }
        item = CrearItem(contenido, nombre, tipo, size, clave);
        auxiliar->sig = item;
        return 0;
    }
}

//Funcion que elimina un item y libera su memoria
void EliminarItem(nodo ** item){
    if((strcmp((*item)->tipo, "malloc")==0)||(strcmp((*item)->tipo, "HISTORIAL")==0)){
        free((*item)->valor);
    } else if(strcmp((*item)->tipo, "mmap")==0){
        munmap((*item)->valor, (*item)->size);
    } else if(strcmp((*item)->tipo, "shared memory")==0){
        shmdt((*item)->valor);
    }
    free(*item);
}

//Funcion que recorre la lista y va eliminando sus elementos
void EliminarLista(nodo ** lista){
    nodo * auxiliar = *lista;
    while((*lista)!= NULL){
        (*lista) = (*lista)->sig;
        EliminarItem(&auxiliar);
        auxiliar = (*lista);
    }
    (*lista) = NULL;
}

//Estas funciones son solo para listar el historial, no válidas para punteros, y podría cascar
//Funcion que muestra por pantalla los elementos de la lista
void Listar(nodo * lista, char tipo[20]){
    int i = 1;
    nodo * auxiliar = lista;
    while(auxiliar!=NULL){
        if((tipo == NULL)||(strcmp(auxiliar->tipo, tipo)==0)){
            if((tipo != NULL)&&(strcmp(tipo, "HISTORIAL")==0)){
                printf("%d->%s", i, (char *) auxiliar->valor);
            } else {
                char aux [100];
                struct tm * tm = localtime(&(auxiliar->fecha));
                strftime(aux, 100, "%c", tm);
                printf("%p Size:%d %s", auxiliar->valor, auxiliar->size, auxiliar->tipo);
                if (strcmp(auxiliar->tipo, "mmap")==0) 
                   printf(" %s (fd: %d)",auxiliar->nombre, 1);
                if (strcmp(auxiliar->tipo, "shared memory")==0)
                    printf(" (key:%d)", auxiliar->clave);
                printf(" %s\n", aux);
            }
            i++;
        }
        auxiliar = auxiliar->sig;
    }
    return;
}

void ListarN(nodo * lista, int posiciones){
    int i = 1;
    if(posiciones < 1){
        printf("Posicion no valida\n");
        return;
    } else {
        nodo * auxiliar = lista;
        for (i=1; i<= posiciones; i++){
            if(auxiliar!=NULL){
                printf("%d->%s", i, (char *) auxiliar->valor);
                auxiliar = auxiliar->sig;
            } else return;
        }
        return;
    }
}

char * Buscar(nodo * lista, int posicion){
    int i = 1;
    if(posicion < 1){
        printf("Posicion no valida\n");
        return NULL;
    } else {
        nodo * auxiliar = lista;
        while((auxiliar != NULL)&&(i<posicion)){
            auxiliar = auxiliar->sig;
            i++;
        }
        if (i==posicion){
            return auxiliar->valor;
        } else printf("Posicion no valida\n");
        return NULL;
    }
}

int BuscarClave(nodo * lista, key_t clave, char * tipo){
    nodo * auxiliar = lista;
    while(auxiliar != NULL){
        if ((strcmp(auxiliar->tipo, tipo)==0)&&(auxiliar->clave == clave)){
            return 1;
        }
        auxiliar = auxiliar->sig;
    }
    return 0;
}

int eliminarSize(nodo ** lista, int size, char * tipo){
    if(*lista!=NULL){
        nodo * auxiliar = *lista;
        if((strcmp(auxiliar->tipo, tipo)==0)&&(auxiliar->size == size)){
            *lista = auxiliar->sig;
            printf("deallocated %d at %p\n", auxiliar->size, auxiliar->valor);
            EliminarItem(&auxiliar);
            return 0;
        } else {
            nodo * anterior = *lista;
            auxiliar = auxiliar->sig;
            while(auxiliar!=NULL){
                if((strcmp(auxiliar->tipo, tipo)==0)&&(auxiliar->size == size)){
                    if(auxiliar->sig == NULL){
                        anterior->sig = NULL;
                        printf("deallocated %d at %p\n", auxiliar->size, auxiliar->valor);
                        EliminarItem(&auxiliar);
                    } else{
                        anterior->sig = auxiliar->sig;
                        printf("deallocated %d at %p\n", auxiliar->size, auxiliar->valor);
                        EliminarItem(&auxiliar);
                    }
                    return 0;
                }
                auxiliar = auxiliar->sig;
                anterior = anterior->sig;
            }
            printf("No hay reservas del tipo %s de tamaño %d\n", tipo, size);
            return 1;
        }
    } else {
        printf("No hay reservas de memoria\n");
        return 1;
    }
}

int eliminarNombre(nodo ** lista, char * nombre, char * tipo){
    if(*lista!=NULL){
        nodo * auxiliar = *lista;
        if((strcmp(auxiliar->tipo, tipo)==0)&&(strcmp(auxiliar->nombre, nombre)==0)){
            *lista = auxiliar->sig;
            printf("deallocated %s at %p\n", auxiliar->nombre, auxiliar->valor);
            EliminarItem(&auxiliar);
            return 0;
        } else {
            nodo * anterior = *lista;
            auxiliar = auxiliar->sig;
            while(auxiliar!=NULL){
                if((strcmp(auxiliar->tipo, tipo)==0)&&(strcmp(auxiliar->nombre, nombre)==0)){
                    if(auxiliar->sig == NULL){
                        anterior->sig = NULL;
                        printf("deallocated %s at %p\n", auxiliar->nombre, auxiliar->valor);
                        EliminarItem(&auxiliar);
                    } else{
                        anterior->sig = auxiliar->sig;
                        printf("deallocated %s at %p\n", auxiliar->nombre, auxiliar->valor);
                        EliminarItem(&auxiliar);
                    }
                    return 0;
                }
                auxiliar = auxiliar->sig;
                anterior = anterior->sig;
            }
            printf("No esta el fichero: %s mapeado en memoria\n", nombre);
            return 1;
        }
    } else {
        printf("No hay reservas de memoria\n");
        return 1;
    }
}

//Elimina una posicion de la lista si la direccion de memoria coincide
//Si hay tiempo cambiar
int eliminarDireccion(nodo ** lista, char * dir){
    if(*lista!=NULL){
        nodo * auxiliar = *lista;
        void * p = (void *) strtoul(dir, NULL, 16);
        if(p==auxiliar->valor){
            *lista = auxiliar->sig;
            printf("block at address %p deallocated (%s)\n", auxiliar->valor,auxiliar->tipo);
            EliminarItem(&auxiliar);
            return 0;
        } else {
            nodo * anterior = *lista;
            auxiliar = auxiliar->sig;
            while(auxiliar!=NULL){
                if(p==auxiliar->valor){
                    if(auxiliar->sig == NULL){
                        anterior->sig = NULL;
                        printf("block at address %p deallocated (%s)\n", auxiliar->valor,auxiliar->tipo);
                        EliminarItem(&auxiliar);
                    } else{
                        anterior->sig = auxiliar->sig;
                        printf("block at address %p deallocated (%s)\n", auxiliar->valor,auxiliar->tipo);
                        EliminarItem(&auxiliar);
                    }
                    return 0;
                }
                auxiliar = auxiliar->sig;
                anterior = anterior->sig;
            }
            printf("Direccion %s no reservada con malloc, mmap o shared\n", dir);
            return 1;
        }
    } else {
        printf("No hay reservas de memoria\n");
        return 1;
    }
}

//Funcion que elimina por clave 
int eliminarClave(nodo ** lista, key_t clave, char * tipo){
    if(*lista!=NULL){
        nodo * auxiliar = *lista;
        if((strcmp(auxiliar->tipo, tipo)==0)&&(auxiliar->clave == clave)){
            *lista = auxiliar->sig;
            printf("block at adress %p deallocated (%s)\n", auxiliar->valor, auxiliar->tipo);
            EliminarItem(&auxiliar);
            return 0;
        } else {
            nodo * anterior = *lista;
            auxiliar = auxiliar->sig;
            while(auxiliar!=NULL){
                if((strcmp(auxiliar->tipo, tipo)==0)&&(auxiliar->clave == clave)){
                    if(auxiliar->sig == NULL){
                        anterior->sig = NULL;
                        printf("block at adress %p deallocated (%s)\n", auxiliar->valor, auxiliar->tipo);
                        EliminarItem(&auxiliar);
                    } else{
                        anterior->sig = auxiliar->sig;
                        printf("block at adress %p deallocated (%s)\n", auxiliar->valor, auxiliar->tipo);
                        EliminarItem(&auxiliar);
                    }
                    return 0;
                }
                auxiliar = auxiliar->sig;
                anterior = anterior->sig;
            }
            printf("No hay reservas del tipo %s con la clave %d\n", tipo, clave);
            return 1;
        }
    } else {
        printf("No hay reservas de memoria\n");
        return 1;
    }
}

//Funciones para usar como iterador
pos first(nodo * lista){
    return lista;
}

//Preguntar para que es: typedef struct node * pos
pos next(pos entrada){
    return entrada->sig;
}

int ultimo(pos entrada){
    return entrada==NULL;
}

void * element(pos entrada){
    return entrada->valor;
}

/*
int main(){
    lista lista = CrearLista();
    int tam = 10;
    void * p = malloc(tam*sizeof(char));
    rItem(&lista, p, "item", "item", 10);
    EliminarLista(&lista);
    free(p);
    return 0;
}*/