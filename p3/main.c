/* 

Authors:
    
    Santiago Julio Iglesias Portela 
    Manuel Pérez Sueiro

Logins:
    
    santiago.iglesias4@udc.es   
    manuel.psueiro@udc.es
    
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <sys/utsname.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>
#include <dirent.h>
#include <errno.h>
#include <pwd.h>
#include <grp.h>
#include "main.h"
#include "process.c"
#include "memory.c"


int CommandSplit(char * cadena, char * trozos[]) {
    
    int i=1;
    if ((trozos[0]=strtok(cadena," \n\t"))==NULL){
        return 0;
    }
    while ((trozos[i]=strtok(NULL," \n\t"))!=NULL){
        i++;
    }
    return i;
}

void pid (char * argmnt[], int aux){
    if(aux > 2){
        printf("Invalid number of arguments");
    }else{
    if (argmnt[1] == NULL){
        printf("Pid del shell: %d\n", getpid());
    } else if (strcmp(argmnt[1], "-p") == 0){
        printf("Pid del padre del shell: %d\n",getppid());
    } else perror("Argumento no válido\n");
    }
}

void prompt(){
    printf(">> ");  // promt
}

void authors(char * argmnt[], int aux){
    
    if(aux>2){
        printf("Invalid number of arguments\n");
    }
    else{
    if (argmnt[1] == NULL){
        printf("Santiago J. Iglesias Portela --> santiago.iglesias4@udc.es\nManuel Pérez Sueiro --> manuel.psueiro@udc.es\n");
    }
    else if (strcmp(argmnt[1], "-l") == 0){
        printf("santiago.iglesias4@udc.es\nmanuel.psueiro@udc.es\n");
    }
    else if (strcmp(argmnt[1], "-n") == 0){
        printf("Santiago J. Iglesias Portela\nManuel Pérez Sueiro\n");
    }else perror("Argumento no valido\n");
    }
    
}

void fecha(char * argmnt[], struct tm tm, int aux){
    if(aux > 2){
        printf("Invalid number of arguments");
    }
    else{
    if (argmnt[1] == NULL){
        printf("Fecha actual: %02d/%02d/%d\n", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
        printf("Hora actual: %02d:%02d:%02d\n", tm.tm_hour, tm.tm_min, tm.tm_sec);
    }
    else if(strcmp(argmnt[1], "-d") == 0){
        printf("Fecha actual: %02d/%02d/%d\n", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
    }
    else if(strcmp(argmnt[1], "-h") == 0){
        printf("Hora actual: %02d:%02d:%02d\n", tm.tm_hour, tm.tm_min, tm.tm_sec);
    }
    else perror("Argumento no valido");
    }
}


void carpeta(char * armgnt[], int aux){
    
    if(aux > 2){
        printf("Invalid number of arguments");
    }
    else{
        char buffer[MAX_DIR_SIZE];
    if (getcwd(buffer, MAX_DIR_SIZE)==NULL){                 
         perror("El directorio supera el máximo de memoria");
    }
    else{
        if (armgnt[1] == NULL){
            printf("%s\n", getcwd(buffer, MAX_DIR_SIZE));
        }
        else if (chdir(armgnt[1]) == 0){
            printf("%s\n", getcwd(buffer, MAX_DIR_SIZE));
        }
        else{
            printf("Invalid directory\n");    
        }
    }
    }
}

void infosis(char * argmnt[], int aux){
    
    if(aux > 2){
        printf("Invalid number of arguments");
    }
    else{
    struct utsname name;
    int uname(struct utsname *name);   
    if(argmnt[1] != NULL){
        printf("No arguments allowed");
    }
    else{
        if (uname(&name) < 0){
        perror("Failure uname()");
        }
        else{
        printf("Nodename: %s\n", name.nodename);
        printf("Machine:  %s\n", name.machine);
        printf("Sysname:  %s\n", name.sysname);
        printf("Version:  %s\n", name.version);
        printf("Release:  %s\n", name.release);
        }
    }
    }
}

void ayuda(char * argmnt[], int aux){
    if(aux > 2){
        printf("Invalid number of arguments");
    }
    else{
    if (argmnt[1] == NULL){
        printf("\nCOMMANDS\n\nautores\npid\ncarpeta\nfecha\nhist\ncomando\ninfosis\nayuda\nfin\nbye\nsalir\n");
    }
    else if (strcmp(argmnt[1], "autores") == 0){
        printf("\nCommand: %s\n\nPrints the names and logins of the program authors\n\n ARGUMENTS\n\n(-l) only prints the logins\n(-n) only prints the names\n\n", argmnt[1]);
    }
    else if (strcmp(argmnt[1], "pid") == 0){
        printf("\nCommand: %s\n\nPrints the pid of the process executing the shell\n\n ARGUMENTS\n\n(-p) Prints the pidof the shell’s parent process\n\n", argmnt[1]);
    }
    else if (strcmp(argmnt[1], "carpeta") == 0){
        printf("\nCommand: %s [direct]\n\nChanges the current working directory of the shell to direct \n\n ARGUMENTS\n\nWith no arguments prints the current working directory\n\n", argmnt[1]);
    }
    else if (strcmp(argmnt[1], "fecha") == 0){
        printf("\nCommand: %s\n\nprints the current date and the current time \n\n ARGUMENTS\n\n(-d) Prints the current date in format\n(-h) Prints the current time in the format hh:mm:ss \n\n", argmnt[1]);
    }
    else if (strcmp(argmnt[1], "hist") == 0){
        printf("\nCommand: %s\n\nPrints all the comands that have been input with their order number \n\n ARGUMENTS\n\n(-c) Clears (empties) the list of historic commands  \n(-N) Prints the first N comands\n\n", argmnt[1]);
    }
    else if (strcmp(argmnt[1], "comando") == 0){
        printf("\nCommand: %s N\n\nRepeats command number N (from historic list)  \n\n", argmnt[1]);
    }
    else if (strcmp(argmnt[1], "infosis") == 0){
        printf("\nCommand: %s\n\nPrints information on the machine running the shell \n\n", argmnt[1]);
    }
    else if (strcmp(argmnt[1], "ayuda") == 0){
        printf("\nCommand: %s [cmd]\n\nDisplays a list of available commands\n\n ARGUMENTS\n\n[cmd] ayuda cmd gives a brief help on the usage of comand cmd\n\n", argmnt[1]);
    }
    else if (strcmp(argmnt[1], "fin") == 0 || strcmp(argmnt[1], "bye") == 0 || 
             strcmp(argmnt[1], "salir") == 0){ 
             printf("\nCommand: %s\n\nEnds the shell\n\n", argmnt[1]);
    }
    }
}

bool endConditioning(char * argmnt){
   if (argmnt == NULL){
       return false;
   }
   else{
       if (strcmp(argmnt, "fin") == 0 ||
           strcmp(argmnt, "salir") == 0 || strcmp(argmnt, "bye") == 0) {
           return true;
       }
       else return false;
   }
}

void hist(char * argmnt[], head_t * comandList, int aux){
    
    if(aux > 2){
        printf("Invalid number of arguments");
    }
    else{
    if (argmnt[1] == NULL){
        printList(comandList, INT_MAX, "historial");
    }
    else if (strcmp(argmnt[1], "-c") == 0){
        clearList(comandList);
    }
    else if (atoi(argmnt[1]+1) > 0){
        printList(comandList, atoi(argmnt[1]+1), "historial");
    }
    else{
        printf("Invalid argument");
    }
    }
}

void comando(char * argmnt[], head_t * comandList, int aux, char * env[]){
    
    if(aux > 2){
        printf("Invalid number of arguments");
    }
    else{
    char aux[MAX_CADENA];

    if (argmnt[1] == NULL){
        printf("Please introduce the position of the command you want to repeat");
    }
    else if (atoi(argmnt[1]) > 0){
        strcpy(aux, getNelement(comandList, atoi(argmnt[1]) - 1));
        processComand(comandList, NULL, aux, env);
        printf("\n");
    }
    else perror("Invalid argument");
    }
}

void crear(char * argmnt[], int aux){
    
    if(aux > 3 || aux <= 1){  
        printf("Invalid number of arguments\n");
    }
    else{
        FILE *file;
        struct stat st = {0};
        if(aux == 1){
            char buffer[MAX_DIR_SIZE];
            printf("%s\n", getcwd(buffer, MAX_DIR_SIZE));
        }
        else if(strcmp(argmnt[1], "-f")==0 && aux == 3){
            file = fopen(argmnt[2], "w");
            printf("File '%s' created succesfully\n", argmnt[2]);
        }  
        else if(stat(argmnt[1], &st) == -1 && aux == 2 && mkdir(argmnt[1],0777) == 0){ // checkear si existe o no el directorio, preguntar profe si mkdir hace comprobacion de existencia de directorio
            printf("Directory %s succesfully created\n", argmnt[1]);
        }
        else perror("Unable to create an actual existing element\n");
    }
}

int isFile(const char* name){
    DIR* directory = opendir(name);

    if(directory != NULL){
     closedir(directory);
     return 0;
    }

    if(errno == ENOTDIR) return 1;

    return -1;
}

void borrar(char * argmnt[], int aux, int op){                              
    if (op == 1){
    if(aux > 1){
    
        for(int i = 0; i<aux; i++){
        
            if(isFile(argmnt[i]) == 0){      // Directorio     
                if(rmdir(argmnt[i]) == 0){
                    printf("Directory %s succesfully deleted\n", argmnt[i]);
                }else{
                perror("Directory cannot be deleted\n");
                printf("Directory: %s", argmnt[i]);
                }
            }
            else if(unlink(argmnt[i]) == 0){  // File
                printf("File: %s succesfull delete\n", argmnt[i]);
            }else perror("\n");


        }
    }
    else printf("No arguments on command 'borrar'\n");
    
    } // recursivo
    else if(aux == 1){
        char buffer[MAX_DIR_SIZE];
        printf("%s\n", getcwd(buffer, MAX_DIR_SIZE));
    }
    else{
        for (int i = 0; i<aux; i++){
             borrarrec(argmnt[i]);
        }
    }
}

void borrarrec(char * argmnt){
     
        struct stat st;
            
        if (isFile(argmnt) == 0){ // directorio
            DIR * dir;
            struct dirent *dirent;
            dir=opendir(argmnt);

            while ((dirent=readdir(dir))!=NULL){
                    if ((strcmp(dirent->d_name, ".") == 0 || strcmp(dirent->d_name, "..") == 0)){
                        continue;
                    }
                    char * path = malloc(MAX_DIR_SIZE);
                    strcpy(path, argmnt);
                    strcat(path, "/");
                    strcat(path, dirent->d_name);
                    borrarrec(path);
                    free(path);

                    if (rmdir(argmnt)!=-1){
                    printf("Directory %s succesfully deleted\n", argmnt);
                    }
                    else perror("Directory delete error\n");
                    
                    closedir(dir);
                }   
            }
            else{ // no directorio
                if (remove(argmnt)==0){
                printf("File: %s succesfull delete\n", argmnt);
                }
                else perror("File delete error"); 
            }
}
/*
void recASearch(char *dir_name, bool mod, bool link, bool hid, bool pprop) {
    char path[1000];
    struct dirent *dp;
    DIR *dir = opendir(dir_name);
    struct stat stats;

    if (!dir) {
        return;
    }

    strcpy(path, dir_name);

    while ((dp = readdir(dir)) != NULL) {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) {
            if (hid || dp->d_name[0] != '.') {
                if(pprop) {
                    stat(path, &stats);
                    printProperties(stats, path, mod, link);
                } else {
                printf("%s\n", dp->d_name);
                }

                strcpy(path, dir_name);
                strcat(path, "/");
                strcat(path, dp->d_name);

                recASearch(path, mod, link, hid, pprop);
            }
        }
    }

    closedir(dir);
}

void recBSearch(char *dir_name, bool mod, bool link, bool hid, bool pprop) {          //NO TERMINADA
    char path[1000];
    struct dirent *dp;
    DIR *dir = opendir(dir_name);
    struct stat stats;

    if (!dir) {
        return;
    }

    strcpy(path, dir_name);

    while ((dp = readdir(dir)) != NULL) {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) {
            if (hid || dp->d_name[0] != '.') {
                
                strcpy(path, dir_name);
                strcat(path, "/");
                strcat(path, dp->d_name);

                recBSearch(path, mod, link, hid, pprop);
            }
        }
    }
    if(pprop) {
        stat(path, &stats);
        printProperties(stats, path, mod, link);
    } else {
        printf("%s\n", dp->d_name);
    }

    closedir(dir);
}

void openDir(char *dir_name, bool mod, bool link, bool hid, bool pprop) {
    struct dirent *dp;
    DIR *dir;
    struct stat stats;
    char path [1000];

    dir = opendir(dir_name);
    if(dir == NULL) {
        return;
    }

    strcpy(path, dir_name);

    while( (dp=readdir(dir)) ) {
        strcpy(path, dir_name);
        strcat(path, "/");
        strcat(path, dp->d_name);

        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) {
            if (hid || dp->d_name[0] != '.') {
                if(pprop) {
                    stat(path, &stats);
                    printProperties(stats, path, mod, link);
                } else {
                    printf("%s\n", dp->d_name);
                }
            }
        }
    }

    closedir(dir);
}

void listdir(char * argmnt[], int aux) {
    struct stat stats;
    bool longOpt = false, acc = false, link = false, hid = false, reca = false, recb = false;
    int linkCnt = 0, hidCnt = 0, accCnt = 0, longCnt = 0, recaCnt = 0, recbCnt = 0; 
    int optCnt = 0;

    if (aux==1) {
        char buffer[MAX_DIR_SIZE];
        printf("%s\n", getcwd(buffer, MAX_DIR_SIZE));
        return;
    }

    for(int i=1; i<aux; i++) {

        if (strcmp(argmnt[i], "-link") == 0) {
            link=true;
            linkCnt++;
            optCnt++;
        }else if (strcmp(argmnt[i], "-hid") == 0) {
            hid=true;
            hidCnt++;
            optCnt++;
        }else if (strcmp(argmnt[i], "-long") == 0) {
            longOpt=true;
            longCnt++;
            optCnt++;
        }else if (strcmp(argmnt[i], "-acc") == 0) {
            acc = true;
            accCnt++;
            optCnt++;
        }else if (strcmp(argmnt[i], "-reca") == 0) {
            reca=true;
            recaCnt++;
            optCnt++;
        }else if (strcmp(argmnt[i], "-recb") == 0) {
            recb=true;
            recbCnt++;
            optCnt++;
        }

        if ( (argmnt[i][0] == '-') && ( (strcmp(argmnt[i], "-long") != 0) && (strcmp(argmnt[i], "-acc") != 0) && (strcmp(argmnt[i], "-link") != 0) && (strcmp(argmnt[i], "-hid") != 0) && (strcmp(argmnt[i], "-reca") != 0) && (strcmp(argmnt[i], "-recb") != 0) ) ) {
            printf("Option [%s] not found\n", argmnt[i]);
            optCnt++;
        }

    }

    if ( (linkCnt>1) || (hidCnt>1) || (longCnt>1) || (accCnt>1) || (recaCnt>1) || (recbCnt>1) ) {
        printf("Repited options in command 'listdir'\n");
        return;
    }

    if( (recaCnt == 1) && (recbCnt == 1) ) {
        printf("Uncompatible command options in command 'listdir'\n");
        return;
    }

    if (reca) {
        for(int i=optCnt+1; i<aux; i++) {
            if (lstat(argmnt[i], &stats) == 0) {
                if( (S_ISREG(stats.st_mode)) || (S_ISLNK(stats.st_mode)) ){
                    if (longOpt) {
                        printProperties(stats, argmnt[i], acc, link);
                    } else {
                        printf("%s\n", argmnt[i]);
                    }
                }
                recASearch(argmnt[i], acc, link, hid, longOpt);
            } else {
                printf("Cannot access %s: No such file or directory\n", argmnt[i]);
            }
        }
    } else if (recb) {
        for(int i=optCnt+1; i<aux; i++) {
            if (lstat(argmnt[i], &stats) == 0) {
                if( (S_ISREG(stats.st_mode)) || (S_ISLNK(stats.st_mode)) ) {
                    if (longOpt) {
                        printProperties(stats, argmnt[i], acc, link);
                    } else {
                        printf("%s\n", argmnt[i]);
                    }
                }
                recBSearch(argmnt[i], acc, link, hid, longOpt);
            } else {
                printf("Cannot access %s: No such file or directory\n", argmnt[i]);
            }
        }
    } else {
        for(int i=optCnt+1; i<aux; i++) {
            if (lstat(argmnt[i], &stats) == 0) {
                if (hid || argmnt[i][0] != '.') {
                    if (longOpt) {
                        printProperties(stats, argmnt[i], acc, link);
                    } else {
                        printf("%s\n", argmnt[i]);
                    }
                    openDir(argmnt[i], acc, link, hid, longOpt);
                }
            } else {
                printf("Cannot access %s: No such file or directory\n", argmnt[i]);
            }
        }
    }
}
*/




/*
char LetraTF (mode_t m) {
    switch (m&S_IFMT) { 
    case S_IFSOCK: return 's'; 
    case S_IFLNK: return 'l'; 
    case S_IFREG: return '-'; 
    case S_IFBLK: return 'b'; 
    case S_IFDIR: return 'd'; 
    case S_IFCHR: return 'c'; 
    case S_IFIFO: return 'p'; 
    default: return '?'; 
    }
}

/*
char * ConvierteModo (mode_t m, char *permisos) {
    strcpy (permisos,"---------- ");
    permisos[0]=LetraTF(m);
    if (m&S_IRUSR) permisos[1]='r'; 
    if (m&S_IWUSR) permisos[2]='w';
    if (m&S_IXUSR) permisos[3]='x';
    if (m&S_IRGRP) permisos[4]='r'; 
    if (m&S_IWGRP) permisos[5]='w';
    if (m&S_IXGRP) permisos[6]='x';
    if (m&S_IROTH) permisos[7]='r'; 
    if (m&S_IWOTH) permisos[8]='w';
    if (m&S_IXOTH) permisos[9]='x';
    if (m&S_ISUID) permisos[3]='s'; 
    if (m&S_ISGID) permisos[6]='s';
    if (m&S_ISVTX) permisos[9]='t';
    return permisos;
}*/
/*
void printProperties(struct stat stats, char file[], bool acc, bool link){
    struct tm dt;
    char permisos[12];
    if(acc) {
        dt = *(gmtime(&stats.st_atim.tv_sec));
        printf("%04d/%02d/%02d-%02d:%02d", dt.tm_year + 1900, dt.tm_mon, dt.tm_mday, dt.tm_hour, dt.tm_min);
        

    }else {
        dt = *(gmtime(&stats.st_mtim.tv_sec));                                      
        printf("%04d/%02d/%02d-%02d:%02d", dt.tm_year + 1900, dt.tm_mon, dt.tm_mday, dt.tm_hour, dt.tm_min);

    }
    printf("%3ld  %s  %s  %s  %ld", stats.st_nlink, getpwuid(stats.st_uid)->pw_name, getgrgid(stats.st_gid)->gr_name, ConvierteModo(stats.st_mode, permisos), stats.st_size);
    printf("\t%s", file);
    if(link) {
        if(S_ISLNK(stats.st_mode)){
            char buff[128];
            stat(file, &stats);
            if(readlink(file, buff, sizeof(buff)-1) != -1){
                printf(" -> %s", buff);
            } else {
                printf("Error: %s", strerror(errno));
            }
        }
    }
    printf("\n");
}*/
/*
void listfich(char *argmnt[], int aux){
    if(aux == 1){
        char buffer[MAX_DIR_SIZE];
        printf("%s\n", getcwd(buffer, MAX_DIR_SIZE));
        return;
        }
    else {
        struct stat stats;
        bool acc = false, opt = false, link = false,  longOpt = false;
        int optCnt = 0;
        
        for(int i=1; i<aux; i++) {

            if(strcmp(argmnt[i], "-long") == 0) {
                longOpt = true;
                opt = true;
                optCnt++;
            }

            if(strcmp(argmnt[i], "-acc") == 0) {
                acc = true;
                opt = true;
                optCnt++;
            }
            
            if (strcmp(argmnt[i], "-link") == 0) {
                link = true;
                opt = true;
                optCnt++;

            } 
            
            if ( (argmnt[i][0] == '-') && ( (strcmp(argmnt[i], "-long") != 0) && (strcmp(argmnt[i], "-acc") != 0) && (strcmp(argmnt[i], "-link") != 0) ) ) {
                printf("Option [%s] not found\n", argmnt[i]);
                optCnt++;
            }

        }
        if (!opt || !longOpt) {
            for(int i=optCnt+1; i<aux; i++) {
                if (lstat(argmnt[i], &stats) == 0){
                    printf("%s\t%ld\n", argmnt[i], stats.st_size);
                } else {
                    printf("Cannot access %s: No such file or directory\n", argmnt[i]);
                }
            }   
        } else {
            for(int i=optCnt+1; i<aux; i++) {
                if (lstat(argmnt[i], &stats) == 0){
                    printProperties(stats, argmnt[i], acc, link);
                } else {
                    printf("Cannot access %s: No such file or directory\n", argmnt[i]);
                }
            }
        }
    }
}

*/
bool processComand(head_t * comandList, head_t * memoryList, char * petition, char * env[]){
    
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    void * petitionAux;
    char * argument [MAX_CADENA];
    char Command [MAX_CADENA];

    petitionAux = strdup(petition);

    int aux = CommandSplit(petition, argument);   
    
    if (strcmp(petition, "comando") == 0 || strcmp(petition, "hist") == 0 || strcmp(petition, "\n")==0); // Evitar bucle infinito con command 
                                                                                                         // no registrar el comando hist/command y saltos de linea al ser llamado        
    else Insert(comandList, (char *)petitionAux, 0, 0, 0, "historial", "", 0);

    strcpy(Command, petition);

    if(strcmp(petition, "\n")==0){   // Entradas vacías a la terminal no implica un error
        return false;
    }
    
    else{ 
        if (endConditioning(Command)) {
            return true;
        }
        else if (strcmp(Command, "autores") == 0) {
            authors(argument, aux);
        }
        else if (strcmp(Command, "pid") == 0) {
            pid(argument, aux); 
        }
        else if (strcmp(Command, "fecha") == 0) {
            fecha(argument, tm, aux);
        }
        else if (strcmp(Command, "carpeta") == 0){
            carpeta(argument, aux);
        }
        else if (strcmp(Command, "infosis") == 0){
            infosis(argument, aux);
        }
        else if (strcmp(Command, "ayuda") == 0){
            ayuda(argument, aux);
        }
        else if (strcmp(Command, "hist") == 0){
            hist(argument, comandList, aux);
        }
        else if (strcmp(Command, "comando") == 0){
            comando(argument, comandList, aux, env);
        }
        else if (strcmp(Command, "crear") == 0){
            crear(argument, aux);
        }
        else if (strcmp(Command, "borrar") == 0){
            borrar(argument, aux, 1);
        }/*
        else if (strcmp(Command, "listfich") == 0) {
            listfich(argument, aux);
        }
        else if(strcmp(Command, "listdir") == 0) {
            listdir(argument, aux);
        }
        else if(strcmp(Command, "borrarrec") == 0){
            borrar(argument, aux, 2);
        }
        */
        else if(strcmp(Command, "malloc") == 0){
            fun_malloc(argument, memoryList, aux);
        }
        else if(strcmp(Command, "mmap") == 0){
            fun_mmap(argument, memoryList, aux);
        }
        else if(strcmp(Command, "shared") == 0){
            fun_shared(argument, memoryList, aux);
        }
        else if(strcmp(Command, "dealloc") == 0){
            fun_dealloc(argument, memoryList, aux);
        }
        else if (strcmp(Command, "memoria") == 0){
            fun_memoria(argument, memoryList, aux);
        }
        else if(strcmp(Command, "llenarmem") == 0){
            fun_llenarmem(argument, aux);
        }
        else if(strcmp(Command, "volcarmem") == 0){
            fun_volcarmem(argument, aux);
        }
        else if(strcmp(Command, "recursiva") == 0){
            fun_recursiva(atoi(argument[1]), aux);
        }
        else if((strcmp(Command, "e-s") == 0) && (strcmp(argument[1], "read") == 0)){
            fun_esread(argument+1, aux);
        }
        else if((strcmp(Command, "e-s") == 0) && (strcmp(argument[1], "write") == 0)){
            fun_eswrite(argument+1, aux);
        }
        else if((strcmp(Command, "uid") == 0)){
            fun_uid(argument, aux);
        }
        else if((strcmp(Command, "priority") == 0)){
            fun_priority(argument, aux);
        }
        else if((strcmp(Command, "entorno") == 0)){
            fun_entorno(argument, aux, env);
        }
        else if((strcmp(Command, "mostrarvar") == 0)){
            fun_mostrarvar(argument, aux, env);
        }
        else if((strcmp(Command, "cambiarvar") == 0)){
            fun_cambiarvar(argument, aux, env);
        }
        else{
            fprintf(stderr, "Command: %s not found\n", Command);
        }
    }
    return false;
}

int main(int argc, char * argv[], char * env[]){

    char petition[MAX_CADENA];
    head_t * comandList = NULL;
	createList(&comandList);
    head_t * memoryList = NULL;
	createList(&memoryList);
    bool end = false;
    
    while (!end){
    prompt();
    fgets(petition, sizeof(petition), stdin);
    end = processComand(comandList, memoryList, petition, env);
    }

    removeList(&memoryList);
    removeList(&comandList);
    
    return 0;
}
