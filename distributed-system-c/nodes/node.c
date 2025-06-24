#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "word_counter.h"

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("[i] Uso: %s <PUERTO>\n", argv[0]);
        return 1;
    }

    int my_port = atoi(argv[1]);
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(my_port);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 5);
    printf("[+] Nodo escuchando en el puerto %d\n", my_port);

    // Construir el path del archivo de salida
    char filename[256];
    snprintf(filename, sizeof(filename), "NodeFiles/Input_server_files/Input_%d.txt", my_port);

    while (1) {
        int addrlen = sizeof(address);
        int new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
        if (new_socket < 0) {
            perror("[-] Error al aceptar conexión");
            continue;
        }

        // Abrir archivo para escribir el buffer recibido
        FILE *f = fopen(filename, "w");
        if (!f) {
            perror("fopen");
            close(new_socket);
            continue;
        }

        // Recibir datos del servidor y guardar en archivo
        char buffer[BUFFER_SIZE];
        ssize_t bytes;
        while ((bytes = recv(new_socket, buffer, BUFFER_SIZE, 0)) > 0) {
            fwrite(buffer, 1, bytes, f);
        }

        fclose(f);

        // Procesar el archivo recibido
        words_counter(filename, my_port);

        // Enviar el archivo de lista de palabras al servidor
        char list_filename[256];
        snprintf(list_filename, sizeof(list_filename), "NodeFiles/words_lists/list_%d.txt", my_port);

        FILE *list_file = fopen(list_filename, "r");
        if (list_file) {
            char file_buffer[1024];
            size_t read_bytes;
            while ((read_bytes = fread(file_buffer, 1, sizeof(file_buffer), list_file)) > 0) {
                send(new_socket, file_buffer, read_bytes, 0);
            }
            fclose(list_file);
        }
        shutdown(new_socket, SHUT_RDWR);
        close(new_socket);
    }

    close(server_fd);
    return 0;
}