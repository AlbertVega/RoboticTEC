#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/stat.h>

#include "./ServerUtils/utils.h"
#include "./node_manager.h"

#define PORT 9000
#define BUFFER_SIZE 1024


/**
 * Funcion para desencriptar el contenido del archivo usando XOR
 * 
 * @param arg Puntero a la estructura que contiene el socket del cliente
 */
void *manage_flow(void *arg) {
    
    int new_socket = *(int *)arg;
    free(arg);

    FILE *fp = fopen("ServerFiles/Input_files/archivo_decifrado.txt", "wb");
    FILE *fp_enc = fopen("ServerFiles/Input_files/archivo_cifrado.txt", "wb");
    char buffer[BUFFER_SIZE];
    char key;

    recv(new_socket, &key, sizeof(key), 0);                             // Recibe la clave de desencriptacion

    ssize_t bytes;
    while ((bytes = recv(new_socket, buffer, BUFFER_SIZE, 0)) > 0)      // Mientras haya datos en el buffer que recibir
    {
        fwrite(buffer, 1, bytes, fp_enc);                               // Guarda los datos cifrados en el archivo correspondiente
        xor_decrypt(buffer, bytes, key);                                // Desencripta los datos recibidos usando XOR
        fwrite(buffer, 1, bytes, fp);                                   // Guarda los datos desencriptados en el archivo correspondiente
    }

    fclose(fp);
    fclose(fp_enc);
    close(new_socket);

    printf("Archivo recibido guardado.\n");

    split_file("ServerFiles/Input_files/archivo_decifrado.txt");        // Llama a la funcion para dividir el archivo en partes aproximadamente iguales

    create_nodes();                                                     // Crea los nodos que se encargaran de procesar las partes del archivo

    return NULL;
}


/**
 * * Funcion principal del servidor que crea el socket, escucha conexiones entrantes y maneja cada cliente en un hilo separado.
 */
int main() {

    // Crea el socket del servidor
    int server_fd;
    struct sockaddr_in address;
    socklen_t addr_size = sizeof(address);
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("[-] Creacion del socket fallida");
        return 1;
    }

    // Configura la direccion del servidor
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Asocia el socket a la direccion y puerto, si el bind falla, cierra el socket
    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 5);

    printf("[+] Servidor escuchando en el puerto %d\n", PORT);

    while (1)                                                                           // Espera conexiones entrantes
    {
        int new_socket = accept(server_fd, (struct sockaddr *)&address, &addr_size);    // Acepta una nueva conexion entrante
        if (new_socket < 0) {
            perror("[-] Accept fallido");
            continue;
        }

        int *new_socket_fd = malloc(sizeof(int));                                       // Reserva memoria para el socket del cliente                   
        *new_socket_fd = new_socket;

        pthread_t thread_id;                                                            // Crea un nuevo hilo para manejar la conexion del cliente
        pthread_create(&thread_id, NULL, manage_flow, (void *)new_socket_fd);           // Pasa el socket del cliente al hilo
        pthread_detach(thread_id);                                                      // Desvincula el hilo para que se limpie automaticamente al finalizar
    }

    // Cierra el socket del servidor al finalizar
    close(server_fd);
    return 0;
}

// Nota: El codigo para el main toma como referencia los siguientes enlaces:
//       https://www.youtube.com/watch?v=io2G2yW1Qk8
//       https://idiotdeveloper.com/tcp-client-server-programming-c-guide/