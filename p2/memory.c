#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <errno.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <sys/wait.h>

#define LEERCOMPLETO ((ssize_t)-1)


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


void * MmapFichero (char * fichero, int protection){

    int df, map=MAP_PRIVATE,modo=O_RDONLY;
    struct stat s;
    void *p;

    if (protection&PROT_WRITE) modo=O_RDWR;

    if (stat(fichero,&s)==-1 || (df=open(fichero, modo))==-1)
        return NULL;

    if ((p=mmap (NULL,s.st_size, protection,map,df,0))==MAP_FAILED)
        return NULL;
    

    /*Guardar Direccion de Mmap (p, s.st_size,fichero,df......);*/
    
    return p;
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
    
    if (aux == 1) printList(memoryList, INT_MAX);
    else{
        if (aux == 2){
            if (atoi(argmnt[1]) <= 0){
                printf("size not valid\n");
            }
            else{
                void * mem = malloc(atoi(argmnt[1]));
                printf("allocated: %d size at %p\n", atoi(argmnt[1]), mem);
                Insert(memoryList, mem, atoi(argmnt[1]), 0, 0, "malloc", "");
            }
        }
        if (strcmp(argmnt[1], "-free") == 0){
            if (aux == 2) printList(memoryList, INT_MAX);
            else if (aux == 3 && atoi(argmnt[2])>0) deleteMemoryAtSize(memoryList, atoi(argmnt[2]));
        }

    }
}

void fun_mmap(char *argmnt[], head_t * memoryList, int aux){ /*arg[0] is the file name and arg[1] is the permissions*/

    char *perm;
    void *p;
    int protection=0;
    
    if (aux == 1){ /*Listar Direcciones de Memoria mmap;*/ 
        printList(memoryList, INT_MAX);
        return;
    }

    if ((perm=argmnt[2])!=NULL && strlen(perm)<4) {
        if (strchr(perm,'r')!=NULL) protection|=PROT_READ;
        if (strchr(perm,'w')!=NULL) protection|=PROT_WRITE;
        if (strchr(perm,'x')!=NULL) protection|=PROT_EXEC;
    }

    if ((p=MmapFichero(arg[1],protection))==NULL)
        perror ("Imposible mapear fichero");
    else
        Insert(memoryList, p, atoi(argmnt[1]), 0, 0, "mmap", argmnt[1]);
        printf ("file %s mapped at %p\n", argmnt[1], p);
    }