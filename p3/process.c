#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pwd.h>
#include <signal.h>
#include <sys/resource.h>
#include <errno.h>
#include "linkedList.h"
#include "main.h"

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

void fun_entorno(char * argmnt[], int aux) {
        
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