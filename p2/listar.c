//LIBRERIAS
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

//CONSTANTES
#define LONG "-long"
#define DIRR "-dir"
#define HIDDEN "-hid"
#define REC "-rec"
#define DIR_SIZE 100

//FUNCIONES
char LetraTF (mode_t m)
{
    switch (m&S_IFMT) { /*and bit a bit con los bits de formato,0170000 */
        case S_IFSOCK:  return 's';     /*socket */
        case S_IFLNK:   return 'l';     /*symbolic link*/
        case S_IFREG:   return '-';     /*fichero normal*/
        case S_IFBLK:   return 'b';     /*block device*/
        case S_IFDIR:   return 'd';     /*directorio */
        case S_IFCHR:   return 'c';     /*char  device*/
        case S_IFIFO:   return 'p';     /*pipe*/
        default: return '?';    /*desconocido, no deberia aparecer*/
    }
}

char * ConvierteModo2 (mode_t m)
{
    static char permisos[12];
    strcpy (permisos,"---------- ");

    permisos[0]=LetraTF(m);
    if (m&S_IRUSR) permisos[1]='r';     /*propietario*/
    if (m&S_IWUSR) permisos[2]='w';
    if (m&S_IXUSR) permisos[3]='x';
    if (m&S_IRGRP) permisos[4]='r';     /*grupo*/
    if (m&S_IWGRP) permisos[5]='w';
    if (m&S_IXGRP) permisos[6]='x';
    if (m&S_IROTH) permisos[7]='r';     /*resto*/
    if (m&S_IWOTH) permisos[8]='w';
    if (m&S_IXOTH) permisos[9]='x';
    if (m&S_ISUID) permisos[3]='s';     /*setuid, setgid y stickybit*/
    if (m&S_ISGID) permisos[6]='s';
    if (m&S_ISVTX) permisos[9]='t';
    return (permisos);
}

//Funcion entrada listar
int cmd_listar(char * tokens[], int ntokens, int opciones[]){
    int i = 1;
    while(i<ntokens){
        if (tokens[i][0]=='-'){
            if(strcmp(tokens[i], LONG)==0) opciones[0] = 1;
            if(strcmp(tokens[i], DIRR)==0) opciones[1] = 1;
            if(strcmp(tokens[i], HIDDEN)==0) opciones[2] = 1;
            if(strcmp(tokens[i], REC)==0) opciones[3] = 1;
            i++;
        } else break;
    }
    return i;
}

void ListarDetalles(char * nombre, int lng){
    struct stat st;
    if(lstat(nombre, &st)==0){
        char lnk [st.st_size + 1];
        switch (LetraTF(st.st_mode)){
            case '-':
                if(lng==1){
                    struct tm * tm = localtime(&st.st_mtim.tv_sec);
                    char aux [100];
                    strftime(aux, 100, "%b %d %H:%M", tm);
                    printf("\t%s", aux);
                    printf("\t%ld", st.st_ino);
                    struct passwd * pw = getpwuid(st.st_uid);
                    struct group * grp = getgrgid(st.st_gid);
                    printf("\t%s\t%s", pw->pw_name, grp->gr_name);
                    printf("\t%s", ConvierteModo2(st.st_mode));
                }
                printf("\t%ld", st.st_size);
                if(lng==1) printf("\t(%ld)", st.st_nlink);
                printf("\t%s\n", nombre);
                break;
            case 'd':
                if(lng==1){
                    struct tm * tm = localtime(&st.st_mtim.tv_sec);
                    char aux [100];
                    strftime(aux, 100, "%b %d %H:%M", tm);
                    printf("\t%s", aux);
                    printf("\t%ld", st.st_ino);
                    struct passwd * pw = getpwuid(st.st_uid);
                    struct group * grp = getgrgid(st.st_gid);
                    printf("\t%s\t%s", pw->pw_name, grp->gr_name);
                    printf("\t%s", ConvierteModo2(st.st_mode));
                }
                printf("\t%ld", st.st_size);
                if(lng==1) printf("\t(%ld)", st.st_nlink);
                printf("\t%s\n", nombre);
                break;
            case 'l':
                if(lng==1){
                    struct tm * tm = localtime(&st.st_mtim.tv_sec);
                    char aux [100];
                    strftime(aux, 100, "%b %d %H:%M", tm);
                    printf("\t%s", aux);
                    printf("\t%ld", st.st_ino);
                    struct passwd * pw = getpwuid(st.st_uid);
                    struct group * grp = getgrgid(st.st_gid);
                    printf("\t%s\t%s", pw->pw_name, grp->gr_name);
                    printf("\t%s", ConvierteModo2(st.st_mode));
                }
                printf("\t%ld", st.st_size);
                if(lng==1) printf("\t(%ld)", st.st_nlink);
                printf("\t%s -> ", nombre);
                readlink(nombre, lnk, st.st_size + 1);
                lnk[st.st_size]= '\0';
                printf("%s\n",lnk);
                break;

        }
    }
    return;
}


void list(char * nombre, int opciones[]){
    int lng = opciones[0];
    int dr = opciones[1];
    int hid = opciones[2];
    int rec = opciones[3];
    struct stat st;
    if(lstat(nombre, &st)==0){
        switch (LetraTF(st.st_mode)){
            //FICHERO
            case '-':
                ListarDetalles(nombre, lng);
                break;
            //DIRECTORIO
            case 'd':
                if((dr==1)||((rec == 1)&&(dr == 1))){
                    DIR * d;
                    d=opendir(nombre);
                    struct dirent *ent;
                    while((ent=readdir(d))!=NULL){
                        if((strcmp(ent->d_name, ".")==0)||(strcmp(ent->d_name, "..")==0)){
                            if(rec == 1)continue;
                        }
                        if(ent->d_name[0]=='.'){
                            if(hid == 0)continue;
                        }
                        char *path = malloc(PATH_MAX);
                        strcpy(path,nombre);
                        strcat(path, "/");
                        strcat(path, ent->d_name);
                        struct stat buf;
                        lstat(path, &buf);
                        if(!S_ISDIR(buf.st_mode)){
                            ListarDetalles(path, lng);
                        } else continue;
                        free(path);
                    }
                    rewinddir(d);
                    while((ent=readdir(d))!=NULL){
                        if((strcmp(ent->d_name, ".")==0)||(strcmp(ent->d_name, "..")==0)){
                            if(rec == 1)continue;
                        }
                        if(ent->d_name[0]=='.'){
                            if(hid == 0)continue;
                        }
                        char *path = malloc(PATH_MAX);
                        strcpy(path,nombre);
                        strcat(path, "/");
                        strcat(path, ent->d_name);
                        struct stat buf2;
                        lstat(path, &buf2);
                        if(S_ISDIR(buf2.st_mode)){ 
                            ListarDetalles(path, lng);
                            if(rec == 1) list(path, opciones);
                        } else continue;
                        free(path);
                    }
                    closedir(d);
                } else {
                    //Listar directorio
                    ListarDetalles(nombre, lng);
                }
                break;
                    
            //LINK
            case 'l':
                ListarDetalles(nombre, lng);
                break;
            case '?':
                printf("cannot access %s: No such file or directory\n", nombre);
                break;
        }
    } else {
        printf("Cannot access %s: %s\n", nombre, strerror(errno));
    }
}    

void shell_listar(char * tokens[], int ntokens){
    int opciones [4] = {0,0,0,0};
    int i;
    i = cmd_listar(tokens, ntokens, opciones);
    if (i == ntokens){
        char buffer[DIR_SIZE]; 
        opciones[1]=1;
        if(getcwd(buffer, DIR_SIZE)!=NULL){
            list(buffer, opciones);
        }
    } else {
        for(;i<ntokens; i++){ 
            list(tokens[i], opciones);
        }
    }
}

//Funcion que busca si el borrado es recursivo o no
int cmd_delete(char * tokens[], int ntokens){
    if((ntokens>=2)&&(strcmp(tokens[1], REC)==0)){
        return 1;
    } else return 0;
}

//Funcion recursiva de borrado
void del(char* nombre, int rec){
    struct stat st;
    lstat(nombre, &st);
    if(S_ISDIR(st.st_mode)){
        if(rec==1){
            DIR * d;
            d=opendir(nombre);
            struct dirent *ent;
            while((ent=readdir(d))!=NULL){
                if((strcmp(ent->d_name, ".")==0)||(strcmp(ent->d_name, "..")==0)){
                    continue;
                }
                char *path = malloc(PATH_MAX);
                strcpy(path,nombre);
                strcat(path, "/");
                strcat(path, ent->d_name);
                del(path, rec);
                free(path);
            }
            if(rmdir(nombre)!=-1){
                printf("%s ha sido borrado\n", nombre);
            } else {
                perror(strerror(errno));
            }
            closedir(d);
        } else {
            if(rmdir(nombre)!=-1){
                printf("%s ha sido borrado\n", nombre);
            } else perror(strerror(errno));
        }
    } else if(unlink(nombre)!=-1){
        printf("%s ha sido borrado\n", nombre);
    } else perror(strerror(errno));
}

//Funcion que maneja el tipo de borrado de la shell
void borrar(char * tokens[], int ntokens, int recursivo){
    int i;
    if(recursivo==1){
        if(tokens[2]==NULL){
            int auxiliar [4] = {0,1,0,0};
            list(".", auxiliar);
        } else {
            for(i=2; i< ntokens; i++){
                del(tokens[i], recursivo);
            }
        }
    } else {
        if(tokens[1]==NULL){
            int auxiliar [4] = {0,1,0,0};
            list(".", auxiliar);
        } else {
            for(i=1; i< ntokens; i++){
                del(tokens[i], recursivo);
            }
        }
    }
}
