#ifndef __LISTAR_H__
#define __LISTAR_H__
void list(char * nombre, int opciones[]);
int cmd_delete(char * tokens[], int ntokens);
void shell_listar(char * tokens[], int ntokens);
void borrar(char * tokens[], int ntokens, int recursivo);
#endif