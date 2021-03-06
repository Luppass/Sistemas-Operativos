#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pwd.h>
#include <signal.h>
#include <sys/resource.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include "linkedList.h"
#include "main.h"

#define MAXVAR 2000 //Borrar, solo para hacer prueba
/******************************SENALES ******************************************/
struct SEN{
        char *nombre;
        int senal;
};


static struct SEN sigstrnum[]={
        "HUP", SIGHUP,
        "INT", SIGINT,
        "QUIT", SIGQUIT,
        "ILL", SIGILL,
        "TRAP", SIGTRAP,
        "ABRT", SIGABRT,
        "IOT", SIGIOT,
        "BUS", SIGBUS,
        "FPE", SIGFPE,
        "KILL", SIGKILL,
        "USR1", SIGUSR1,
        "SEGV", SIGSEGV,
        "USR2", SIGUSR2,
        "PIPE", SIGPIPE,
        "ALRM", SIGALRM,
        "TERM", SIGTERM,
        "CHLD", SIGCHLD,
        "CONT", SIGCONT,
        "STOP", SIGSTOP,
        "TSTP", SIGTSTP,
        "TTIN", SIGTTIN,
        "TTOU", SIGTTOU,
        "URG", SIGURG,
        "XCPU", SIGXCPU,
        "XFSZ", SIGXFSZ,
        "VTALRM", SIGVTALRM,
        "PROF", SIGPROF,
        "WINCH", SIGWINCH,
        "IO", SIGIO,
        "SYS", SIGSYS,
/*senales que no hay en todas partes*/
#ifdef SIGPOLL
        "POLL", SIGPOLL,
#endif
#ifdef SIGPWR
        "PWR", SIGPWR,
#endif
#ifdef SIGEMT
        "EMT", SIGEMT,
#endif
#ifdef SIGINFO
        "INFO", SIGINFO,
#endif
#ifdef SIGSTKFLT
        "STKFLT", SIGSTKFLT,
#endif
#ifdef SIGCLD
        "CLD", SIGCLD,
#endif
#ifdef SIGLOST
        "LOST", SIGLOST,
#endif
#ifdef SIGCANCEL
        "CANCEL", SIGCANCEL,
#endif
#ifdef SIGTHAW
        "THAW", SIGTHAW,
#endif
#ifdef SIGFREEZE
        "FREEZE", SIGFREEZE,
#endif
#ifdef SIGLWP
        "LWP", SIGLWP,
#endif
#ifdef SIGWAITING
        "WAITING", SIGWAITING,
#endif
        NULL,-1,
}; /*fin array sigstrnum*/

extern char ** environ;

extern FILE *stderr;

int Senal(char * sen) {
        int i;
    for (i=0; sigstrnum[i].nombre!=NULL; i++)
        if (!strcmp(sen, sigstrnum[i].nombre))
            return sigstrnum[i].senal;
    return -1;
}

char *NombreSenal(int sen) {/*devuelve el nombre senal a partir de la senal para sitios donde no hay sig2str*/
        int i;
        for (i=0; sigstrnum[i].nombre!=NULL; i++)
                if (sen==sigstrnum[i].senal)
                        return sigstrnum[i].nombre;
        return ("SIGUNKNOWN");
}

char * NombreUsuario(uid_t uid) {
        struct passwd *p;
        if ((p=getpwuid(uid))==NULL)
                return (" ??????");
        return p->pw_name;
}

uid_t UidUsuario(char * nombre) {
        struct passwd *p;
        if ((p=getpwnam (nombre))==NULL)
                return (uid_t) -1;
        return p->pw_uid;
}

void MostrarUidsProceso(void) {
        uid_t real=getuid(), efec=geteuid();
        printf ("Credencial real: %d, (%s)\n", real, NombreUsuario (real));
        printf ("Credencial efectiva: %d, (%s)\n", efec, NombreUsuario (efec));
}

void CambiarUidLogin(char * login) {
        uid_t uid;
        if ((uid=UidUsuario(login))==(uid_t) -1){
                printf("login no valido: %s\n", login);
                return;
        }
        if (setuid(uid)==.1)
                printf ("Imposible cambiar credencial: %s\n", strerror(errno));
}

void fun_uid(char * argmnt[], int aux) {
        if( (aux == 1) || ((aux == 2) && (strcmp(argmnt[1], "-get") == 0)) ) {
                MostrarUidsProceso();
        } else {
                if(strcmp(argmnt[1], "-set") == 0) {
                        if(aux == 3) {
                                struct passwd *p;
                                uid_t uid =(uid_t) atoi(argmnt[2]);
                                p=getpwuid(uid);
                                CambiarUidLogin(p->pw_name);
                        }else if((aux == 4) && (strcmp(argmnt[2], "-l") == 0)) {
                                CambiarUidLogin(argmnt[3]);
                        } else {
                                printf("Too much arguments on command: 'uid'\n");
                        }
                } else {
                        printf("Command option not found\n");
                }
        }
}

void fun_priority(char * argmnt[], int aux) {
        int which = PRIO_PROCESS;
        int prio;
        if(aux == 1) {
                id_t pids;
                pids = getpid();
                errno = 0;
                prio = getpriority(which, pids);
                if(errno == 0)
                        printf("Priority of %d: %d\n", pids, prio);
                else
                        printf("Imposible to get priority of process %d: %s\n", pids, strerror(errno));
        } else {
                id_t pid;
                pid = (id_t) atoi(argmnt[1]);
                if(aux == 2) {
                        errno = 0;
                        prio = getpriority(which, pid);
                        if(errno == 0)
                                printf("Priority of %d: %d\n", pid, getpriority(which, pid));
                        else
                                printf("Imposible to get priority of process %d: %s\n", pid, strerror(errno)); 
                } else if (aux == 3) {
                        prio = atoi(argmnt[2]);
                        if(setpriority(which, pid, prio) == -1)
                                printf("Imposible to change priority of process %d: %s\n", pid, strerror(errno));
                } else {
                        printf("Invalid number of arguments in command: 'priority'\n");
                }
        }
}

void MostrarEntorno (char **entorno, char * nombre_entorno) {
        int i=0;
        while (entorno[i]!=NULL) {
                printf ("%p->%s[%d]=(%p) %s\n", &entorno[i], nombre_entorno, i,entorno[i],entorno[i]);
                i++;
        }
}

int BuscarVariable (char * var, char *e[]) {
        int pos=0;
        char aux[MAXVAR];
        strcpy (aux,var);
        strcat (aux,"=");
        while (e[pos]!=NULL)
                if (!strncmp(e[pos],aux,strlen(aux)))
                        return (pos);
                else
                        pos++;
        errno=ENOENT; /*no hay tal variable*/
        return(-1);
}

int CambiarVariable(char * var, char * valor, char *e[]) {
        int pos;
        char *aux;
        if ((pos=BuscarVariable(var,e))==-1)
                return(-1);
        if ((aux=(char *)malloc(strlen(var)+strlen(valor)+2))==NULL)
                return -1;
        strcpy(aux,var);
        strcat(aux,"=");
        strcat(aux,valor);
        e[pos]=aux;
        return (pos);
}

void fun_entorno(char * argmnt[], int aux, char *env[]) {
        if(aux > 2) {
                printf("Too much arguments in command 'entorno'");
        } else {
                if(aux == 1) {                                 //printea el array de enviroment por el tercer argumento del main
                        MostrarEntorno(env, "main 3arg");
                } else if(strcmp(argmnt[1], "-environ") == 0) {         //printea el array de enviroment por la variable externa environ
                        MostrarEntorno(environ, "environ");
                } else if(strcmp(argmnt[1], "-addr") == 0) {            //printea el valor de environ y el tercer argumento del main 
                        printf("environ:\t%p (stored in %p)\n", environ, &environ);
                        printf("main 3arg:\t%p (stored in %p)\n", env, &env);
                } else {
                        printf("Use: entorno [-environ|-addr]\n");
                }
        }
}

void fun_mostrarvar(char * argmnt[], int aux, char *env[]) {
        if(aux == 1) {
                MostrarEntorno(env, "main 3arg");
        } else if(aux == 2) {
                if(getenv(argmnt[1]) == NULL) {
                        printf("Enviroment variable %s not found", argmnt[1]);
                } else {
                        int pos;
                        pos = BuscarVariable(argmnt[1], env);
                        printf("With arg3 main: %s (%p) %p\n", env[pos], env[pos], &env[pos]);
                        printf("With environ: %s (%p) %p\n", environ[pos], environ[pos], &environ[pos]);
                        printf("With getenv: %s=%s (%p)\n", argmnt[1], getenv(argmnt[1]), getenv(argmnt[1]));
                }
        } else {
                printf("Invalid number of arguments in command: 'mostrarvar'\n");
        }
}

void fun_cambiarvar(char * argmnt[], int aux, char *env[]) {
        char a[MAXVAR];
        if(aux < 3) {
                printf("Use: cambiarvar [-a|-e|-p] VAR VALUE\n");
        } else {
                if(strcmp(argmnt[1], "-e") == 0) {
                        CambiarVariable(argmnt[2], argmnt[3], environ);
                } else if(strcmp(argmnt[1], "-a") == 0) {
                        CambiarVariable(argmnt[2], argmnt[3], env);
                } else if(strcmp(argmnt[1], "-p") == 0) {
                        strcpy(a, argmnt[2]);
                        strcat(a, "=");
                        strcat(a, argmnt[3]);
                        putenv(a);
                } else {
                        strcpy(a, argmnt[1]);
                        strcat(a, "=");
                        strcat(a, argmnt[2]);
                        putenv(a);           
                }
        }
}

void fun_ejec(char * argmnt[], int aux) {
        if (execvp(argmnt[1], argmnt+1)==-1){
                printf("%s\n", strerror(errno));
        }

}

void fun_ejecpri(char * argmnt[], int aux) {
        int which = PRIO_PROCESS;
        id_t pid = getpid();
        int prio = atoi(argmnt[1]);
        if(setpriority(which, pid, prio) == -1)
                printf("Imposible to change priority of process %d: %s\n", pid, strerror(errno));
        fun_ejec(argmnt+1, aux-1);
}

void fun_ejecas(char * argmnt[], int aux) {
        CambiarUidLogin(argmnt[1]);
        fun_ejec(argmnt+1, aux-1);
}

void fun_fork(){
        pid_t pid;
        if(pid = fork() == -1){
                printf("%s", strerror(errno));
                return;
        }
        else {
              waitpid(pid, 0, 0); 
        }
}

void fun_fg(char * argmnt[], int aux) {
        int pid;

        pid = fork();
        
        if(pid == -1) {
                printf("%s", strerror(errno));
                return;
        }

        if(pid == 0) {
                if(execvp(argmnt[1], argmnt+1) == -1){
                        printf("%s\n", strerror(errno));
                }
                exit(255);
        }
        waitpid(pid, NULL, 0);
}

void fun_fgprio(char * argmnt[], int aux) {
        int which = PRIO_PROCESS;
        id_t pid = getpid();
        int prio = atoi(argmnt[1]);
        if(setpriority(which, pid, prio) == -1)
                printf("Imposible to change priority of process %d: %s\n", pid, strerror(errno));
        fun_fg(argmnt+1, aux-1);
}

void fun_fgas(char * argmnt[], int aux) {
        CambiarUidLogin(argmnt[1]);
        fun_fg(argmnt+1, aux-1);
}

void fun_back(char * argmnt[], int aux) {
        int pid;

        pid = fork();
        
        if(pid == -1) {
                printf("%s", strerror(errno));
                return;
        }

        if(pid == 0) {
                if(execvp(argmnt[1], argmnt+1) == -1){
                        printf("%s\n", strerror(errno));
                }
                exit(255);
        }
        
}

void fun_backprio(char * argmnt[], int aux) {
        int which = PRIO_PROCESS;
        id_t pid = getpid();
        int prio = atoi(argmnt[1]);
        if(setpriority(which, pid, prio) == -1)
                printf("Imposible to change priority of process %d: %s\n", pid, strerror(errno));
        fun_back(argmnt+1, aux-1);
}

void fun_backas(char * argmnt[], int aux) {
        CambiarUidLogin(argmnt[1]);
        fun_back(argmnt+1, aux-1);
}

void fun_rederr(char * argmnt[], int aux) {
        int fd, backup, status;

        backup=dup(STDOUT_FILENO);
        if(aux == 1) {
                //Mustra a d??nde est?? mandando el error estandar
        } else {
                if(strcmp(argmnt[1], "-reset") == 0) {
                        //Resetea a donde se manda el error estandar
                        close(STDERR_FILENO);
                        dup(backup);
                } else {
                        //Nuevo file a donde se manda el error estandar 
                        if ((fd=open(argmnt[1], O_CREAT|O_EXCL|O_WRONLY ,0777))==-1){
                                perror("Unable to open target file for redirection");
                                exit(0);
                        }
                        close(STDERR_FILENO);
                        dup(fd);
                }
        }
}