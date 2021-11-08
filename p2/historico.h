#ifndef __HISTORICO_H__
#define __HISTORICO_H__
typedef struct nodo * lista;

lista CrearLista();
int InsertarItem(lista *, void * contenido, char nombre[20], char tipo[20], int size, key_t clave);
void EliminarLista(lista *);
void Listar(lista, char tipo[20]);
void ListarN(lista, int posiciones);
char * Buscar(lista, int posicion);
int BuscarClave(lista, key_t clave, char * tipo);
int eliminarSize(lista *, int size, char * tipo);
int eliminarNombre(lista *, char * nombre, char * tipo);
int eliminarDireccion(lista *, char * dir);
int eliminarClave(lista *, key_t clave, char * tipo);

#endif