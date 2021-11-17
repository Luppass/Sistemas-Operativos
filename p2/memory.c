#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/shm.h>
#include <errno.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <sys/wait.h>
#include "linkedList.c"
#include "main.h"

#define TAMANO 4096
#define LEERCOMPLETO ((ssize_t)-1)
#define MAX_CADENA 50
#define MAX_DIR_SIZE 70

//From: https://www.dc.fi.udc.es/~so-grado/2021-22/sources/MemoryZones.c
char * arrayPunteros[]={"hola","buenos","dias","que","lo","disfrutes",NULL,NULL,NULL};

//*********************FUNCIONES AUXILIARES***********************************************************

void fun_recursiva (int n, int aux){

    if (aux == 2){
        char automatico[TAMANO];
        static char estatico[TAMANO];
        printf ("Parámetro n:%d en %p\n",n,&n);
        printf ("Array estático en:%p \n",estatico);
        printf ("Array automático en %p\n\n",automatico);
        n--;
        if (n>0) fun_recursiva(n, aux);
    }
    else printf("Invalid number of arguments");

}


void * ObtenerMemoriaShmget (key_t clave, size_t tam){ /*Obtienen un puntero a una zaona de memoria compartida*/
                                                       /*si tam >0 intenta crearla y si tam==0 asume que existe*/
    void * p;
    int aux,id,flags=0777;
    struct shmid_ds s;
    if (tam) /*si tam no es 0 la crea en modo exclusivo ,esta funcion vale para shared y shared -create*/
        flags=flags | IPC_CREAT | IPC_EXCL;

/*si tam es 0 intenta acceder a una ya creada*/
    if (clave==IPC_PRIVATE) /*no nos vale*/{
        errno=EINVAL; return NULL;
        }
    
    if ((id=shmget(clave, tam, flags))==-1)

    return (NULL);
    
    if ((p=shmat(id,NULL,0))==(void*) -1){
    
    aux=errno; /*si se ha creado y no se puede mapear*/
    
    if (tam) /*se borra */
        shmctl(id,IPC_RMID,NULL);
    
    errno=aux;

    return (NULL);
}
    shmctl (id,IPC_STAT,&s);
    /* Guardar En Direcciones de Memoria Shared (p, s.shm_segsz, clave.....);*/
    return (p);
}


void SharedCreate (char *arg[]) /*arg[0] is the keyand arg[1] is the size*/{
    key_t k;
    size_t tam=0;
    void *p;

    if (arg[0]==NULL || arg[1]==NULL) {/*Listar Direcciones de Memoria Shared */ 
        return;
    }
    
    k=(key_t) atoi(arg[0]);

    if (arg[1]!=NULL)
        tam=(size_t) atoll(arg[1]);

    if ((p=ObtenerMemoriaShmget(k,tam))==NULL)
        perror ("Imposible obtener memoria shmget");
    
    else printf ("Memoria de shmget de clave %d asignada en %p\n",k,p);
}

#define LEERCOMPLETO ((ssize_t)-1)
    
ssize_t LeerFichero (char *fich, void *p, ssize_t n){ /* le n bytes del fichero fich en p */
    ssize_t nleidos,tam=n; /*si n==-1 lee el fichero completo*/
    int df, aux;
    struct stat s;
    if (stat (fich,&s)==-1 || (df=open(fich,O_RDONLY))==-1)
        return ((ssize_t)-1);
        
    if (n==LEERCOMPLETO)
        tam=(ssize_t) s.st_size;
        
    if ((nleidos=read(df,p, tam))==-1){
        aux=errno;
        
    close(df);
    errno=aux;
    return ((ssize_t)-1);
    }
    close (df);
    return (nleidos);
}

void SharedDelkey (char *args[]) /*arg[0] points to a str containing the key*/{
    key_t clave;
    int id;
    char *key=args[0];

    if (key==NULL || (clave=(key_t) strtoul(key,NULL,10))==IPC_PRIVATE){
        printf (" shared -delkey clave_valida\n");
        return;
    }

    if ((id=shmget(clave,0,0666))==-1){
        perror ("shmget: imposible obtener memoria compartida");
        return;
    }

    if (shmctl(id,IPC_RMID,NULL)==-1)
        perror ("shmctl: imposible eliminar memoria compartida\n");
}


void dopmap (void) /*no arguments necessary*/{ 
    pid_t pid; /*ejecuta el comando externo pmap para */
    char elpid[32]; /*pasandole el pid del proceso actual */
    char *argv[3]={"pmap",elpid,NULL};
    sprintf (elpid,"%d", (int) getpid());

    if ((pid=fork())==-1){
        perror ("Imposible crear proceso");
        return;
    }
    if (pid==0){
        if (execvp(argv[0],argv)==-1)
            perror("cannot execute pmap");
            exit(1);
    }
    waitpid (pid,NULL,0);
}


//*********************IMPLEMENTACIONES FUNCIONES***********************************************************

void fun_malloc(char * argmnt[], head_t * memoryList, int aux){
    
    
    if (aux == 1) printList(memoryList, INT_MAX, "malloc");
    else{
        if (aux == 2){
            if (atoi(argmnt[1]) <= 0){
                printf("size not valid\n");
            }
            else{
                void * mem = malloc(atoi(argmnt[1]));
                printf("allocated: %d size at %p\n", atoi(argmnt[1]), mem);
                Insert(memoryList, mem, atoi(argmnt[1]), 0, 0, "malloc", "", 0);
            }
        }
        if (strcmp(argmnt[1], "-free") == 0){
            if (aux == 2) printList(memoryList, INT_MAX, "malloc");
            
            else if (aux == 3 && atoi(argmnt[2])>0){
                deleteMemoryAtSize(memoryList, atoi(argmnt[2]));
            } 
        }
    }
}


void * MmapFichero (char fichero[20], int protection, head_t * memoryList){

    int df, map=MAP_PRIVATE,modo=O_RDONLY;
    struct stat s;
    void *p;

    if (protection&PROT_WRITE) modo=O_RDWR;

    if (stat(fichero,&s)==-1 || (df=open(fichero, modo))==-1)
        return NULL;

    if ((p=mmap(NULL,s.st_size, protection,map,df,0))==MAP_FAILED)
        return NULL;
    
    /*Guardar Direccion de Mmap (p, s.st_size,fichero,df......);*/
    Insert(memoryList, p, s.st_size, 0, 0, "mmap", fichero, df);

    return p;
}

void fun_mmap(char *argmnt[], head_t * memoryList, int aux){ /*arg[0] is the file name and arg[1] is the permissions*/

    char *perm;
    void *p;
    int protection=0;
    if (aux == 1){ /*Listar Direcciones de Memoria mmap;*/ 
        printList(memoryList, INT_MAX, "mmap");
        return;
    }

    if (strcmp(argmnt[1], "-free") == 0){
        if(aux == 2){
            printList(memoryList, INT_MAX, "mmap");
            return;
        }else{
            void * memdir = searchByFilename(memoryList, INT_MAX, argmnt[2]);
            if (memdir != NULL){
                deleteMemoryAtFilename(memoryList, argmnt[2]);
                return;
            }
        }
    }else{
        if ((perm=argmnt[2])!=NULL && strlen(perm)<4) {
            if (strchr(perm,'r')!=NULL) protection|=PROT_READ;
            if (strchr(perm,'w')!=NULL) protection|=PROT_WRITE;
            if (strchr(perm,'x')!=NULL) protection|=PROT_EXEC;
        }

    if ((p=MmapFichero(argmnt[1],protection, memoryList))==NULL) perror ("Imposible mapear fichero");
        else printf ("file %s mapped at %p\n", argmnt[1], p);
    }
}

void fun_dealloc(char * argmnt[], head_t * memoryList, int aux){
    
    if (aux == 1){
        printList(memoryList, INT_MAX, "MALL");
        return;
    }else{
        if(strcmp(argmnt[1], "-malloc") == 0 && atoi(argmnt[2]) > 0){
            deleteMemoryAtSize(memoryList, atoi(argmnt[2]));
            return;
        }
        if(strcmp(argmnt[1], "-mmap") == 0 && aux == 3){
            deleteMemoryAtFilename(memoryList, argmnt[2]);     
            return;
        }
    }
    if (aux == 2){
        deleteMemoryAtAdress(memoryList, argmnt[1]);
    }
    else printf("Arguments not allowed\n");
}

void memoria_funcs(){
        printf("\nAddress of function malloc() is :%p\n", fun_malloc);
        printf("Address of function mmap() is :%p\n", fun_mmap);
        printf("Address of function dealloc() is :%p\n\n", fun_dealloc);
}

void memoria_vars(){

    static int a = 1; static float b = 1.0; static char c = 'a';
    int d = 2; float e = 2.0; char f = 'b';

    printf("\nAddress of global variable initialized is : %p\n", &arrayPunteros);
    printf("Address of global variable initialized is : %p\n", &arrayPunteros[1]);
    printf("Address of global variable initialized is : %p\n", &arrayPunteros[4]);

    printf("\nAddress of variable type static int 'a' is : %p\n", &a);
    printf("Address of variable type static float 'b' is : %p\n", &b);
    printf("Address of variable type static char 'c' is : %p\n", &c);

    printf("\nAddress of variable type int 'a' is : %p\n", &d);
    printf("Address of variable type float 'b' is : %p\n", &e);
    printf("Address of variable type char 'c' is : %p\n\n", &f);
}

void fun_memoria(char * argmnt[], head_t * memorylist, int aux){
    
    bool BLOCKS = false, VARS = false, FUNCS = false,  ALL = false, PMAP = false;

    if (aux == 1){
        memoria_vars();
        return;
    }else{
    
    for (int i = 1; i < aux; i++){
        if(strcmp(argmnt[i], "-blocks") == 0) BLOCKS = true;
        if(strcmp(argmnt[i], "-vars") == 0) VARS = true;
        if(strcmp(argmnt[i], "-funcs") == 0) FUNCS = true;
        if(strcmp(argmnt[i], "-all") == 0){
            ALL = true; BLOCKS = false; VARS = false; FUNCS = false;
        } 
        if(strcmp(argmnt[i], "-pmap") == 0) PMAP = true;
    }
    
    if (BLOCKS){
        printList(memorylist, INT_MAX, "MALL"); 
        return;
    } 

    if (VARS){
        memoria_vars();
        return;
    }
    if (FUNCS){
        memoria_funcs();
        return;
    } 

    if (ALL){
        printList(memorylist, INT_MAX, "MALL");
        memoria_vars();
        memoria_funcs();
        return;
    }
    if (PMAP){
        printf("\n");
        dopmap();
        printf("\n");
        return;
    }

    printf("Arguments not allowed, bad syntax\n");
    
    }
}

void fun_llenarmem(char * argmnt[], int aux){

        if (aux > 1){
        
        char byte ='A';
        int cont = 128;
        void *p = strdup(argmnt[1]);

        char * dir = (char *) p;

        if(aux == 3) cont = atoi(argmnt[2]);
        
        if(aux == 4) byte = strtol(argmnt[3], NULL, 16);
        
        for(int i=0; i<cont ; i++){
            dir[i] = byte;
        } 
   
    }else printf("Arguments not allowed, bad syntax\n");
}

    


