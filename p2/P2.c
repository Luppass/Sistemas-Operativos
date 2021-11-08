/*
Autores de la practica
    Eva Maria Arce Ale                 LOGIN: eva.arce
    Francisco Javier Espada Radio      LOGIN: j.espada
*/
//Librerias incluidas en la práctica
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <ctype.h>
#include <stdlib.h>
#include "historico.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>
#include "listar.h"
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/shm.h>

//Constantes
#define MAX_CADENA 100
#define MAX_TOKEN 20
#define DIR_SIZE 100
#define NO 0
#define SI 1
#define AUTOR_1 "Francisco Javier Espada Radio"
#define LOGIN_1 " j.espada"
#define AUTOR_2 " Eva Maria Arce Ale"
#define LOGIN_2 " eva.arce"
#define ARGUMENTO_INVALIDO "Argumento no valido"
#define TAMANO_DO_RECURSIVA 4096

//Funciones del shell
#define EXIT "exit"
#define END "end"
#define QUIT "quit"
#define AUTHORS "authors"
#define GETPID "getpid"
#define GETPPID "getppid"
#define PWD "pwd"
#define CHDIR "chdir"
#define DATE "date"
#define TIME "time"
#define HISTORIC "historic"
#define CREATE "create"
#define DELETE "delete"
#define LIST "list"
#define MEMORY "memory"
#define RECURSE "recurse"
#define MEMDUMP "memdump"
#define MEMFILL "memfill"
#define READFILE "readfile"
#define WRITEFILE "writefile"

//tokens de listar
#define LONG "-long"
#define DIRR "-dir"
#define HIDDEN "-hid"
#define REC "-rec"

//funciones de memory
#define ALLOC "-allocate"
#define DEALLOC "-dealloc"
#define DELETEKEY "-deletekey"
#define SHOW "-show"
#define SHOWVARS "-show-vars"
#define SHOWFUNCS "-show-funcs"
#define PMAP "-pmap"

//tokens de memory
#define MALLOC "-malloc"
#define MMAP "-mmap"
#define SHOW "-show"
#define DOPMAP "-dopmap"
#define CREATESHARED "-createshared"
#define SHARED "-shared"
#define DELETEKEY "-deletekey"

//Tokens de memory -show
#define VARS "-show-vars"
#define FUNCS "-show-funcs"
#define ALL "-all"

//Leer fichero flag
#define LEERCOMPLETO ((ssize_t)-1)

//VARIABLES GLOBALES
int variable_global_1 = 0;
int variable_global_2 = 1;
int variable_global_3 = 2;

//Cabeceras
void procesar(int ntokens, char * tokens[], lista * historico, lista * memoria);

//Funcion que imprime el inicio de comando del SHELL
void Prompt(){
    printf("->");
}

//Funcion que separa las cadenas de caracteres
int TrocearCadena(char * cadena, char * trozos[]){
    int i=1;
    if ((trozos[0]=strtok(cadena," \n\t"))==NULL)
        return 0;
    while ((trozos[i]=strtok(NULL," \n\t"))!=NULL)
        i++;
    return i;
}

//Funcion que imprime por pantalla el login y nombre de los autores de la practica
int autores(char argumento[]){
    int control = 0;
    if ((argumento == NULL)||(strcmp(argumento, "-n")==0)){
        printf("Autores: %s\n\t%s\n", AUTOR_1, AUTOR_2);
        control++;
    }
    if ((argumento == NULL)||(strcmp(argumento, "-l")==0)){
        printf("Logins: %s\n\t%s\n", LOGIN_1, LOGIN_2);
        control++;
    }
    if (control==0) printf("%s\n", ARGUMENTO_INVALIDO);
    return 0;
}

//Funcion que devuelve el PID del proceso actual
int pidhijo(void){
    printf("PID del proceso actual: %d\n", getpid());
    return 0;
}

//Funcion que devuelve el PID del padre del proceso actual
int pidpadre(void){
    printf("PID del padre del proceso actual: %d\n", getppid());
    return 0;
}

//Funcion que devuelve el directorio actual
int pwd(void){
    char buffer[DIR_SIZE]; 
    if(getcwd(buffer, DIR_SIZE)==NULL){
        perror(strerror(errno));
    } else printf("El directorio actual es: %s\n", buffer);   
    return 0;
}

//Funcion que cambia de directorio
int changedir(char * directory){
    if (directory == NULL){
        pwd();
        return 0;
    } else {
        if (chdir(directory)==0){
            pwd();
            return 0;
        } else {
            perror(strerror(errno));
            return 0;
        }
    }
}

//Funcion que dependiendo del input imprime la data o la hora (0 es data y 1 es hora)
void tiempo(int input){
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    if (input == 0){
        printf("Fecha actual: %02d/%02d/%d\n", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
    } else {
        printf("Hora actual: %02d:%02d:%02d\n", tm.tm_hour, tm.tm_min, tm.tm_sec);
    }
}

//Funcion de historial
void historial(lista * historico, char * opcion, int num, lista * memoria){
    int auxiliar = 0;
    int ntokens;
    char * tokens[MAX_CADENA];
    if (num == 1){
        Listar(*historico, "HISTORIAL");
        return;
    } else if(strcmp(opcion, "-c")==0){
        EliminarLista(historico);
        printf("Historial eliminado\n");
        return;
    } else if(opcion[0]=='-'){
        if(isdigit(opcion[1])!=0){
            auxiliar = atoi(opcion++);
            ListarN(*historico, auxiliar);
            return;
        } else if((opcion[1]=='r')&&(isdigit(opcion[2])!=0)){
            auxiliar = atoi(opcion+2);
            ntokens = TrocearCadena(Buscar(*historico, auxiliar), tokens);
            procesar(ntokens, tokens, historico, memoria);
            return;
        }
    } else {
        printf("%s\n", ARGUMENTO_INVALIDO);
        return;
    }
    return;
}

//Funcion para crear archivos o directorios
void crear(char * tokens[], int palabras){
    if(palabras == 1){
        int auxiliar [4] = {0,1,0,0};
        list(".", auxiliar);
    }else if(palabras == 2){
            if(strcmp(tokens[1], DIRR)==0){
                int auxiliar [4] = {0,1,0,0};
                list(".", auxiliar);
            }else{
                int fd;
                if ((fd = open(tokens[1], O_CREAT|O_WRONLY|O_TRUNC, S_IRUSR|S_IWUSR|S_IROTH))!=-1){
                    close(fd);
                } else{
                    printf("No se ha podido crear el fichero: %s\n", tokens[1]);
                }
            }
    } else {
        if((palabras==3)&&(strcmp(tokens[1], "-dir")==0)){
            if(mkdir(tokens[2], S_IRUSR|S_IWUSR|S_IXUSR)!=-1){
                printf("Se ha creado el directorio: %s\n", tokens[2]);
            } else perror(strerror(errno));
        } else printf("%s\n", ARGUMENTO_INVALIDO);
    }
}

//Funciones proporcionadas por el enunciado MMAP
void * MmapFichero (char * fichero, int protection, lista * memoria){
    int df, map=MAP_PRIVATE,modo=O_RDONLY;
    struct stat s;
    void *p;
    if (protection&PROT_WRITE)  modo=O_RDWR;
    if (stat(fichero,&s)==-1 || (df=open(fichero, modo))==-1) return NULL;
    if ((p=mmap (NULL,s.st_size, protection,map,df,0))==MAP_FAILED) return NULL;
    /*Guardar Direccion de Mmap (p, s.st_size,fichero,df......);*/
    InsertarItem(memoria, p, fichero,"mmap", s.st_size, 0);
    return p;
}

void Cmd_AllocateMmap (char *arg[], lista * memoria) /*arg[0] is the file nameand arg[1] is the permissions*/{
    char *perm;
    void *p;
    int protection=0;
    if (arg[0]==NULL){
        /*Listar Direcciones de Memoria mmap;*/ 
        Listar(*memoria, "mmap");
        return;
    }
    if ((perm=arg[1])!=NULL && strlen(perm)<4) {
        if (strchr(perm,'r')!=NULL) protection|=PROT_READ;
        if (strchr(perm,'w')!=NULL) protection|=PROT_WRITE;
        if (strchr(perm,'x')!=NULL) protection|=PROT_EXEC;
    }
    if ((p=MmapFichero(arg[0],protection, memoria))==NULL){
        printf("Fichero: %s\n", arg[0]);
        perror ("Imposible mapear fichero");
    }
    else printf ("fichero %s mapeado en %p\n", arg[0], p);
    
}

ssize_t LeerFichero (char *fich, void *p, ssize_t n){   /*n=-1 indica que se lea todo*/
    ssize_t  nleidos,tam=n;
    int df, aux;
    struct stat s;
    if (stat (fich,&s)==-1 || (df=open(fich,O_RDONLY))==-1)
        return ((ssize_t)-1);
    if (n==LEERCOMPLETO)tam=(ssize_t) s.st_size;
    if ((nleidos=read(df,p, tam))==-1){
        aux=errno;
        close(df);
        errno=aux;
        return ((ssize_t)-1);
    }
    close (df);
    return (nleidos);
}

void Cmd_dopmap (char *args[]){ /*no arguments necessary*/ 
    pid_t pid;
    char elpid[32];
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

void * ObtenerMemoriaShmget (key_t clave, size_t tam, lista * memoria){
    void * p;
    int aux,id,flags=0777;
    struct shmid_ds s;
    if (tam)  /*si tam no es 0 la crea en modo exclusivo */
        flags=flags | IPC_CREAT | IPC_EXCL;
        /*si tam es 0 intenta acceder a una ya creada*/
    if (clave==IPC_PRIVATE){   /*no nos vale*/
        errno=EINVAL;
        return NULL;
    }
    if ((id=shmget(clave, tam, flags))==-1)return (NULL);
    if ((p=shmat(id,NULL,0))==(void*) -1){
        aux=errno;   /*si se ha creado y no se puede mapear*/
        if (tam)     /*se borra */
            shmctl(id,IPC_RMID,NULL);
        errno=aux;
        return (NULL);
    }
    shmctl (id,IPC_STAT,&s);
    /* Guardar En Direcciones de Memoria Shared (p, s.shm_segsz, clave.....);*/
    InsertarItem(memoria, p, "nombre", "shared memory", s.shm_segsz, clave);
    return (p);
}

void Cmd_AlocateCreateShared (char *arg[], lista * memoria){ /*arg[0] is the keyand arg[1] is the size*/
    key_t k;
    size_t tam=0;
    void *p;
    if (arg[0]==NULL || arg[1]==NULL){/*Listar Direcciones de Memoria Shared */ 
        Listar(*memoria, "shared memory");
        return;
    }
    k=(key_t) atoi(arg[0]);
    if (arg[1]!=NULL)
        tam=(size_t) atoll(arg[1]);
    if ((p=ObtenerMemoriaShmget(k,tam, memoria))==NULL)
        perror ("Cannot allocate: No such file or directory");
    else printf ("Allocated shared memory (key %d) at %p\n",k,p);
}

void Cmd_deletekey (char *args[]){ /*arg[0] points to a str containing the key*/
    key_t clave;
    int id;
    char *key=args[0];
    if (key==NULL || (clave=(key_t) strtoul(key,NULL,10))==IPC_PRIVATE){
        printf ("   rmkey  clave_valida\n");
        return;
    }
    if ((id=shmget(clave,0,0666))==-1){
        perror ("shmget: imposible obtener memoria compartida");
        return;
    }
    if (shmctl(id,IPC_RMID,NULL)==-1)
        perror ("shmctl: imposible eliminar memoria compartida\n");
}

//Funciones externas a memory
void doRecursiva (int n){
    char automatico[TAMANO_DO_RECURSIVA];
    static char estatico[TAMANO_DO_RECURSIVA];
    printf ("parametro n:%d en %p\t",n,&n);
    printf ("array estatico en:%p \t",estatico);
    printf ("array automatico en %p\n",automatico);
    n--;
    if (n>0)
        doRecursiva(n);
}

/******************************************************************************************/
//Funcion show NULL con flags para Vars y funcs
void showNull(int control){
    int a = 0;
    int b = 1;
    int c = 2;
    if((control == 0)||(control == 1)){
        if(control == 0) printf("Funciones del programa: %p\t%p\t%p\n", time, autores, crear);
        printf("Variables locales: %p\t%p\t%p\n", &a, &b, &c);
        printf("Variables globales: %p\t%p\t%p\n", &variable_global_1, &variable_global_2, &variable_global_3);
    }
    if(control == 2){
        printf("Funciones del programa: %p\t%p\t%p\n", time, autores, crear);
        printf("Funciones de librerias: %p\t%p\t%p\n", strcpy, stat, printf);
    }
    return;
}

//Funcion para allocate shared
void Cmd_AlocateShared (char *arg[], lista * memoria){ /*arg[0] is the keyand arg[1] is the size*/
    key_t k;
    size_t tam=0;
    void *p;
    if (arg[0]==NULL){/*Listar Direcciones de Memoria Shared */ 
        Listar(*memoria, "shared memory");
        return;
    }
    k=(key_t) atoi(arg[0]);
    if ((p=ObtenerMemoriaShmget(k,tam, memoria))==NULL)
        perror ("Cannot allocate: No such file or directory");
    else printf ("Allocated shared memory (key %d) at %p\n",k,p);
}

//Funciones de reserva de memeoria
void cmd_alloc(char * tokens[], int ntokens, lista * memoria){
    if(ntokens == 2){
        Listar(*memoria, NULL);
    } else{
        if (strcmp(tokens[2], MALLOC)==0){
            if(ntokens == 4){
                int size = atoi(tokens[3]);
                if(size > 0){
                    void * aux = malloc(size);
                    InsertarItem(memoria, aux, "nombre","malloc", size, 0);
                    printf("allocated %d at %p\n", size, aux);
                } else {
                    printf("Size not valid\n");
                }
            } else {
                Listar(*memoria, "malloc");
            }
            return;
        } else if (strcmp(tokens[2], MMAP)==0){
            Cmd_AllocateMmap(tokens+3, memoria);
            return;
        } else if (strcmp(tokens[2], CREATESHARED)==0){
            Cmd_AlocateCreateShared(tokens+3, memoria);
            return;
        } else if (strcmp(tokens[2], SHARED)==0){
            Cmd_AlocateShared(tokens+3, memoria);
            return;
        } else return;
    }
}
//Funciones de borrado de reservas de memoria
void cmd_dealloc(char * tokens[], int ntokens, lista * memoria){
    if(ntokens==2){
        Listar(*memoria, NULL);
    } else {
        if (strcmp(tokens[2], MALLOC)==0){
            if(ntokens == 4){
                int size = atoi(tokens[3]);
                if(size > 0){
                    eliminarSize(memoria, size, "malloc");
                } else {
                    printf("Size not valid\n");
                }
            } else {
                Listar(*memoria, "malloc");
            }
            return;
        } else if (strcmp(tokens[2], MMAP)==0){
            if(ntokens == 3){
                Listar(*memoria, "mmap");
            } else if(ntokens == 4){
                eliminarNombre(memoria, tokens[3], "mmap");
            }
            return;
        } else if (strcmp(tokens[2], SHARED)==0){
            if(ntokens == 3){
                Listar(*memoria, "shared memory");
            } else if(ntokens == 4){
                eliminarClave(memoria, atoi(tokens[3]), "shared memory");
            }
            return;
        } else if (tokens[2]!= NULL){
            eliminarDireccion(memoria, tokens[2]);
            return;
        }
    }
    
}

//Funcion show de memoria
void cmd_show(char * tokens[], int ntokens, lista * memoria){
    if(ntokens == 2){
        showNull(0);
    } else if(ntokens == 3){
        if(strcmp(tokens[2], ALL)==0){
            Listar(*memoria, NULL);
        } else if(strcmp(tokens[2], MALLOC)==0){
            Listar(*memoria, "malloc");
        } else if(strcmp(tokens[2], MMAP)==0){
            Listar(*memoria, "mmap");
        } else if(strcmp(tokens[2], SHARED)==0){
            Listar(*memoria, "shared memory");
        } else return;
    }    
    return;
}

//Funciones de memoria
void cmd_memory(char * tokens[], int ntokens, lista * memoria){
    if(ntokens == 1){
        Listar(*memoria, NULL);
    } else {
        if (strcmp(tokens[1], ALLOC)==0){
            cmd_alloc(tokens, ntokens, memoria);
            return;
        } else if (strcmp(tokens[1], DEALLOC)==0){
            cmd_dealloc(tokens, ntokens, memoria);
            return;
        } else if (strcmp(tokens[1], SHOW)==0){
            cmd_show(tokens, ntokens, memoria);
            return;
        } else if (strcmp(tokens[1], FUNCS)==0){
            showNull(2);
            return;
        } else if (strcmp(tokens[1], VARS)==0){
            showNull(1);
            return;
        } else if (strcmp(tokens[1], DOPMAP)==0){
            Cmd_dopmap(tokens);
            return;
        } else if (strcmp(tokens[1], DELETEKEY)==0){
            Cmd_deletekey(tokens+2);
            return;
        } else return;
    }
}

//Funcion que procesa la entrada para doRecursiva
void cmd_recursiva(char * tokens[], int ntokens){
    if(ntokens == 2){
        doRecursiva(atoi(tokens[1]));
    }
}

void cmd_memdump(char * tokens[], int ntokens){
    if(ntokens > 1){
        int cont = 25;
        int aux;
        int i = 0;
        int j = 0;
        void * dir = (void *) strtoul(tokens[1], NULL, 16);
        if(ntokens == 3){
            cont = atoi(tokens[2]);
        }
        char * linea1 = (char *) dir;
        unsigned int linea2;
        while(i<cont){
            for(aux = 0;(i<cont)&&(aux<25); i++, aux++){
                if(isprint(linea1[i])){
                    printf("%3c", linea1[i]);
                } else{
                    printf("  ");
                }
            }
            printf("\n");
            for(aux=0;(j<cont)&&(aux<25); j++, aux++){
                linea2 = *((unsigned char *) &linea1[j]);
                printf(" %2x", linea2);
            }
            printf("\n");
        }
    }
}

void cmd_memfill(char * tokens[], int ntokens){
    if(ntokens > 1){
        int cont = 128;
        int i;
        void * dir = (void *) strtoul(tokens[1], NULL, 16);
        char relleno = 'A';
        if(ntokens > 2){
            cont = atoi(tokens[2]);
        }
        if(ntokens == 4){
            relleno = strtol(tokens[3], NULL, 16);;
        }
        char * linea1 = (char *) dir;
        for(i=0;(i<cont); i++){
            linea1[i] = relleno;
        }
    }
}

void cmd_readfile(char * tokens[], int ntokens){
    if(ntokens > 2){
        int cont = -1;
        void * dir = (void *) strtoul(tokens[2], NULL, 16);
        if(ntokens == 4) cont = atoi(tokens[3]);
        printf("leidos %ld bytes de %s en %p\n", LeerFichero(tokens[1], dir, cont), tokens[1], dir);
    } else {
        printf("Not enough arguments\n");
    }
}

ssize_t EscibirFichero (char *fich, void *p, ssize_t n){   /*n=-1 indica que se escriba todo*/
    ssize_t  nescritos,tam=n;
    int df, aux;
    struct stat s;
    if (stat (fich,&s)==-1 || (df=open(fich,O_WRONLY))==-1)
        return ((ssize_t)-1);
    if (n==LEERCOMPLETO)tam=(ssize_t) s.st_size;
    if ((nescritos=write(df,p, tam))==-1){
        aux=errno;
        close(df);
        errno=aux;
        return ((ssize_t)-1);
    }
    close (df);
    return (nescritos);
}

void cmd_writefile(char * tokens[], int ntokens){
    if(ntokens == 4){
        void * dir = (void *) strtoul(tokens[2], NULL, 16);
        int cont = atoi(tokens[3]);
        if(access(tokens[1], F_OK ) == 0 ) {
            printf("Imposible escribir %s: File already exists\n", tokens[1]);
        } else {
            crear(tokens, 2);
            EscibirFichero(tokens[1], dir, cont);
        }
    } else if(ntokens == 5){
        if(strcmp(tokens[1], "-o")==0){
            tokens++;
            crear(tokens, 2);
            void * dir = (void *) strtoul(tokens[2], NULL, 16);
            int cont = atoi(tokens[3]);
            EscibirFichero(tokens[1], dir, cont);
        } else {
            printf("Wrong arguments\n");
        }
    } else {
        printf("Not enough arguments\n");
    }
}

//Funcion que procesa la entrada del usuario
void procesar(int ntokens, char * tokens[], lista * historico, lista * memoria){
    if (strcmp(tokens[0], AUTHORS)==0){
        autores(tokens[1]);
        return;
    } else if (strcmp(tokens[0], GETPID)==0){
        pidhijo();
        return;
    } else if (strcmp(tokens[0], GETPPID)==0){
        pidpadre();
        return;
    } else if (strcmp(tokens[0], PWD)==0){
        pwd();
        return;
    } else if (strcmp(tokens[0], CHDIR)==0){
        changedir(tokens[1]);
        return;
    } else if (strcmp(tokens[0], DATE)==0){
        tiempo(0);
        return;
    } else if (strcmp(tokens[0], TIME)==0){
        tiempo(1);
        return;
    } else if (strcmp(tokens[0], HISTORIC)==0){
        historial(historico, tokens[1], ntokens, memoria);
        return;
    } else if (strcmp(tokens[0], CREATE)==0){
        crear(tokens, ntokens);
        return;
    } else if (strcmp(tokens[0], DELETE)==0){
        borrar(tokens, ntokens, cmd_delete(tokens, ntokens));
        return;
    } else if (strcmp(tokens[0], LIST)==0){
        shell_listar(tokens, ntokens);
        return;
    } else if (strcmp(tokens[0], MEMORY)==0){
        cmd_memory(tokens, ntokens, memoria);
        return;
    } else if (strcmp(tokens[0], RECURSE)==0){
        cmd_recursiva(tokens, ntokens);
        return;
    } else if (strcmp(tokens[0], MEMDUMP)==0){
        cmd_memdump(tokens, ntokens);
        return;
    } else if (strcmp(tokens[0], MEMFILL)==0){
        cmd_memfill(tokens, ntokens);
        return;
    } else if (strcmp(tokens[0], READFILE)==0){
        cmd_readfile(tokens, ntokens);
        return;
    } else if (strcmp(tokens[0], WRITEFILE)==0){
        cmd_writefile(tokens, ntokens);
        return;
    } else {
        printf("COMANDO NO VALIDO\n");
        return;
    }
}

//Funcion principal del SHELL
int main(int argc, char const *argv[]){
    //Variables
    int finalizar = NO;
    char input[MAX_CADENA];
    char * tokens[MAX_TOKEN];
    lista historico;
    historico = CrearLista();
    lista memoria;
    memoria = CrearLista();
    //char copia_peticion[MAX_CADENA];
    void * copia_peticion;
    int ntokens = 0;
    //Loop principal
    while (finalizar == NO){
        //Impresion de inicio de linea y recogida de datos
        Prompt();
        fgets(input, sizeof(input), stdin);
        //strcpy(copia_peticion, input);    //VIEJA
        copia_peticion = strdup(input);
        //Separacion de los datos en tokens
        ntokens = TrocearCadena(input, tokens);
        //Comprobacion de primera palabra para finalizar
        if(ntokens>0){
            if ((strcmp(EXIT, input)==0)||(strcmp(QUIT, input)==0)||(strcmp(END, input)==0))
            {
                finalizar = SI;
                EliminarLista(&historico);
                EliminarLista(&memoria);
            } else {
                procesar(ntokens, tokens, &historico, &memoria);
                InsertarItem(&historico, copia_peticion, "nombre","HISTORIAL", 0, 0);
            }
        }
    }
    free(copia_peticion);
    return 0;
}