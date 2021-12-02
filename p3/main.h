#ifndef main_h
#define main_h

#include "linkedList.h"

// Main functions top level declaration

int CommandSplit(char * cadena, char * trozos[]);
void pid (char * argmt[], int aux);
void prompt();
void authors(char * argmnt[], int aux);
void fecha(char * argmnt[], struct tm tm, int aux);
void carpeta(char * armgnt[], int aux);
void infosis(char * argmnt[],int aux);
void ayuda(char * argmn[],int aux);
bool endConditioning(char * argmnt);
void hist(char * argmnt[], head_t * lista, int aux);
bool processComand(head_t * lista, head_t * memoryList, char * petition);
void comando(char * argmnt[], head_t * lista, int aux);
void crear(char * argmnt[], int aux);
void borrar(char * argmnt[], int aux, int op);
void borrarrec(char * argmnt);
int isFile(const char * argmnt);
void printProperties(struct stat stats, char file[], bool mod, bool link);
void listfich(char *argmnt[], int aux);
void listdir(char *argmnt[], int aux);
void recASearch(char *dir_name, bool mod, bool link, bool hid, bool pprop);
void openDir(char *dir_name, bool mod, bool link, bool hid, bool pprop);
char LetraTF (mode_t m);
char * ConvierteModo (mode_t m, char *permisos);
#endif