#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024


/**
 * Algoritmo de cifrado XOR.
 * 
 * Aplica una operación XOR a cada byte del dato con la clave proporcionada
 */
void xor_encrypt(char *data, size_t len, char key) {
    for (size_t i = 0; i < len; i++) {
        data[i] ^= key;
    }
}

/**
 * Función principal del cliente.
 * 
 * Crea un socket, se conecta al servidor, envía un archivo cifrado usando XOR
 */
int main() {
    int sock;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    // Creamos el socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Creacion del socket fallida");
        return 1;
    }

    // Configuramos la direccion del servidor
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        perror("Direccion IP invalida o no soportada");
        close(sock);
        return 1;
    }

    // Conectamos al servidor
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Conexion al servidor fallida");
        close(sock);
        return 1;
    }

    // Abrimos el archivo a enviar
    FILE *fp = fopen("ClientFiles/el_quijote.txt", "rb");
    if (!fp) {
        perror("Error al abrir el archivo");
        close(sock);
        return 1;
    }

    // Definimos la clave para el cifrado XOR
    char key = 0x5A;

    // Enviamos la clave al servidor
    if (send(sock, &key, sizeof(key), 0) != sizeof(key)) {
        perror("Error al enviar la clave");
        fclose(fp);
        close(sock);
        return 1;
    }

    size_t bytes;

    // Leemos el archivo y lo ciframos antes de enviarlo
    while ((bytes = fread(buffer, 1, BUFFER_SIZE, fp)) > 0) {

        // Ciframos el buffer con la clave
        xor_encrypt(buffer, bytes, key);

        // Enviamos el buffer cifrado al servidor
        ssize_t sent = send(sock, buffer, bytes, 0);

        // Si el tamaño enviado no coincide con el tamaño leído, hay un error
        if (sent != bytes) {
            perror("Error al enviar datos");
            fclose(fp);
            close(sock);
            return 1;
        }
    }

    // Cerramos el archivo
    fclose(fp);
    close(sock);
    printf("Archivo enviado correctamente.\n");
    return 0;
}