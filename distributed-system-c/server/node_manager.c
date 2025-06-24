#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/stat.h>

#include "node_manager.h"
#include "ServerUtils/utils.h"

#define NODES 3


// Estructura para pasar argumentos a los hilos
const int PORTS[NODES] = {9001, 9002, 9003};
const char *FILE_PARTS[NODES] = {
    "ServerFiles/Split_files/part_1.txt",
    "ServerFiles/Split_files/part_2.txt",
    "ServerFiles/Split_files/part_3.txt"
};


/**
 * * Funcion que se ejecuta en cada hilo para enviar partes del archivo a los nodos.
 * *
 * * @param arg Puntero a la estructura NodeArguments que contiene los argumentos del hilo.
 */
void *send_parts(void *arg) {

    NodeArguments *node = (NodeArguments *)arg;                                     // Puntero a la estructura NodeArguments para obterner los argumentos del hilo
    
    // Crea el socket para conectarse al nodo                                         
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in nodo_addr;

    // Configura la direccion del nodo al que se va a enviar la parte del archivo
    nodo_addr.sin_family = AF_INET;
    nodo_addr.sin_port = htons(node->port);
    inet_pton(AF_INET, "127.0.0.1", &nodo_addr.sin_addr);

    // Conecta al nodo
    connect(sock, (struct sockaddr *)&nodo_addr, sizeof(nodo_addr));


    // Envia la parte del archivo al nodo
    FILE *fp = fopen(node->file_part, "r");
    char buffer[1024];

    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), fp)) > 0)                      // Mientras haya datos en el archivo
    {
        send(sock, buffer, bytes, 0);                                               // Envia datos al nodo 
    }

    fclose(fp);
    shutdown(sock, SHUT_WR);


    // Esperar respuesta del nodo, siendo esta la lista de palabras más frecuentes
    printf("[i] Esperando respuesta del nodo en el puerto %d\n", node->port);


    // Al recibir la respuesta del nodo, se guarda en un archivo de lista
    char list_filename[256];
    snprintf(list_filename, sizeof(list_filename), "ServerFiles/Words_lists/list_%d.txt", node->port);

    FILE *list_file = fopen(list_filename, "w");
    if (list_file)                                                                  // Si se pudo abrir el archivo de lista                         
    {
        char buffer_tmp[1024];
        ssize_t bytes;
        while ((bytes = recv(sock, buffer_tmp, sizeof(buffer_tmp), 0)) > 0)         // Mientras haya datos en el buffer temporal del nodo
        {
            fwrite(buffer_tmp, 1, bytes, list_file);                                // Escribe los datos en el archivo de lista
        }
        fclose(list_file);
    }

    close(sock);
    pthread_exit(NULL);
}


/**
 * * Esta funcion crea un hilo por cada nodo, envia la parte del archivo correspondiente
 * * y espera a que todos los hilos terminen antes de continuar.
 * *
 * * Aqui se estaria planteando un sistema distribuido donde cada nodo procesa una parte del archivo
 * * y devuelve la lista de palabras más frecuentes. Se da el paralelismo virtual.
 */
void create_nodes() {

    pthread_t hilos[NODES];                                         // Arreglo de hilos para manejar los nodos
    NodeArguments args[NODES];                                      // Arreglo de estructuras NodeArguments para pasar argumentos a los hilos 

    for (int i = 0; i < NODES; i++)                                 // Por cada nodo
    {
        args[i].port = PORTS[i];                                    // Asigna el puerto del nodo
        args[i].file_part = FILE_PARTS[i];                          // Asigna la parte del archivo correspondiente al nodo
        args[i].response = 0;                                        
        pthread_create(&hilos[i], NULL, send_parts, &args[i]);      // Crea el hilo para enviar la parte del archivo al nodo
    }

    for (int i = 0; i < NODES; i++)                                 // Por cada hilo 
    {
        pthread_join(hilos[i], NULL);                               // Espera a que el hilo termine
        printf("[+] Respuesta recibida del nodo del puerto %d\n", args[i].port);
    }

    most_freq_word(PORTS, NODES);

}
