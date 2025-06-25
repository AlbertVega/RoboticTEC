#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "word_counter.h"

#define BUFFER_SIZE 1024

/**
 * * Funcion principal del nodo que escucha en un puerto específico,
 */
int main(int argc, char *argv[]) {
   
    if (argc != 2)                                                      // Verificar que se haya pasado el puerto como argumento                                                                    
    {
        printf("[i] Uso: %s <PUERTO>\n", argv[0]);
        return 1;
    }

    int my_port = atoi(argv[1]);                                        // Convertir el argumento a un número entero                                                                       

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);                    // Crear un socket TCP
    struct sockaddr_in address;                                         // Estructura para almacenar la dirección del socket

    // Verificar si se pudo crear el socket
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(my_port);

    // Asignar el socket a la dirección y puerto
    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 5);
    printf("[+] Nodo escuchando en el puerto %d\n", my_port);

    // Construir el path del archivo de salida y el nombre del archivo de entrada
    char filename[256];
    snprintf(filename, sizeof(filename), "NodeFiles/Input_server_files/Input_%d.txt", my_port); //

    while (1)                                                           // Bucle infinito para aceptar conexiones entrantes                
    {

        // Aceptar una conexión entrante
        int addrlen = sizeof(address);
        int new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
        if (new_socket < 0) 
        {
            perror("[-] Error al aceptar conexión");
            continue;
        }

        FILE *f = fopen(filename, "w");
        char buffer[BUFFER_SIZE];
        ssize_t bytes;
        while ((bytes = recv(new_socket, buffer, BUFFER_SIZE, 0)) > 0)  // Mientras se reciban datos del socket     
        {
            fwrite(buffer, 1, bytes, f);                                // Escribi los datos recibidos en el archivo
        }

        fclose(f);

        words_counter(filename, my_port);                               // Llamar a la función para contar las palabras en el archivo recibido

        char list_filename[256];
        snprintf(list_filename, sizeof(list_filename), "NodeFiles/words_lists/list_%d.txt", my_port);

        FILE *list_file = fopen(list_filename, "r");

        if (list_file)                                                  // Si el archivo se pudo abrir correctamente
        {
            char file_buffer[1024];
            size_t read_bytes;
            while ((read_bytes = fread(file_buffer, 1, sizeof(file_buffer), list_file)) > 0) // Mientras se puedan leer datos del archivo
            {
                send(new_socket, file_buffer, read_bytes, 0);           // Envia los datos leido del buffer al socket
            }
            fclose(list_file);
        }
        shutdown(new_socket, SHUT_RDWR);
        close(new_socket);
    }

    close(server_fd);
    return 0;
}