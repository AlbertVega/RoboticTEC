#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/stat.h>

#define PORT 8080
#define BUFFER_SIZE 1024


/**
 * Algoritmo de descifrado XOR
 * 
 * Aplica una operación XOR a cada byte del dato con la clave proporcionadaS
 */
void xor_decrypt(char *data, size_t len, char key) {
    for (size_t i = 0; i < len; i++) {
        data[i] ^= key;
    }
}

/**
 * Maneja los datos del cliente
 * 
 * Recibe un archivo cifrado, descifra su contenido usando XOR y guarda tanto el archivo cifrado como el descifrado en el servidor
 */
void *handle_client(void *arg) {
    int client_sock = *(int *)arg;
    free(arg);

    // Aseguramos que el directorio de archivos del servidor existe
    mkdir("ServerFiles", 0777); // Asegura que el directorio existe

    // Abrimos los archivos de salida
    FILE *fp = fopen("ServerFiles/archivo_decifrado.txt", "wb"); // Descifrado
    FILE *fp_enc = fopen("ServerFiles/archivo_cifrado.txt", "wb"); // Cifrado

    // Verificamos que los archivos se abrieron correctamente
    if (!fp || !fp_enc) {
        perror("Error al abrir archivos de salida");
        if (fp) fclose(fp);
        if (fp_enc) fclose(fp_enc);
        close(client_sock);
        return NULL;
    }

    // Buffer para recibir datos
    char buffer[BUFFER_SIZE];

    // Clave para el decifrado XOR
    char key;

    // Recibir la clave del cliente
    ssize_t received = recv(client_sock, &key, sizeof(key), 0);

    // Si el tamaño recibido es diferente a la clave, hubo un error
    if (received != sizeof(key)) {
        perror("Error recibiendo la clave");
        fclose(fp);
        fclose(fp_enc);
        close(client_sock);
        return NULL;
    }

    ssize_t bytes; // Variable para almacenar el numero de bytes recibidos

    // Recibir el archivo cifrado mientras el tamaño de datos sea mayor que 0
    while ((bytes = recv(client_sock, buffer, BUFFER_SIZE, 0)) > 0) {

        fwrite(buffer, 1, bytes, fp_enc); // Guarda el cifrado

        xor_decrypt(buffer, bytes, key); // Descifra el contenido recibido

        fwrite(buffer, 1, bytes, fp); // Guarda el descifrado
    }

    // Si el numero de bytes es negativo, hubo un error en la recepción
    if (bytes < 0) {
        perror("Error recibiendo datos del cliente");
    }

    // Cerramos los archivos
    fclose(fp);
    fclose(fp_enc);
    close(client_sock);

    printf("Archivo recibido guardado.\n");

    return NULL;
}

/**
 * Servidor principal
 */
int main() {
    int server_sock, *client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size = sizeof(client_addr);

    // Creamos el socket del servidor
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("Error creando socket");
        return 1;
    }

    // Configuramos la direccion del servidor
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Asociamos el socket a la direccion y puerto, si el bind falla, cerramos el socket
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error en bind");
        close(server_sock);
        return 1;
    }

    // Escuchamos conexiones entrantes, si el listen falla, cerramos el socket
    if (listen(server_sock, 5) < 0) {
        perror("Error en listen");
        close(server_sock);
        return 1;
    }

    printf("Servidor esperando conexiones...\n");

    // Bucle principal del servidor, acepta conexiones entrantes y crea un hilo para cada cliente
    while (1) {

        // Reservamos memoria para el socket del cliente
        client_sock = malloc(sizeof(int));
        if (!client_sock) {
            perror("Error reservando memoria para client_sock");
            continue;
        }

        // Aceptamos una conexion entrante, si el accept falla, liberamos la memoria y continuamos
        *client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &addr_size);
        if (*client_sock < 0) {
            perror("Error en accept");
            free(client_sock);
            continue;
        }

        // Creamos un hilo para manejar al cliente
        pthread_t tid;
        pthread_create(&tid, NULL, handle_client, client_sock);
        pthread_detach(tid);
    }

    // Cerramos el socket del servidor al finalizar
    close(server_sock);
    return 0;
}