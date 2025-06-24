#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

/**
 * Cifra los datos usando XOR con la clave proporcionada
 * 
 * @param data Puntero al buffer de datos a cifrar
 * @param len Longitud de los datos a cifrar
 * @param key Clave de cifrado
 */
void xor_encrypt(char *data, size_t len, char key) {
    for (size_t i = 0; i < len; i++)                                    // Para cada byte en los datos
    {
        data[i] ^= key;                                                 // Aplica la operacion XOR con la clave                   
    }
}

/**
 * Envia un archivo al servidor cifrado usando XOR
 * 
 * @param sock Socket del cliente conectado al servidor
 * @param filename Nombre del archivo a enviar
 * @param key Clave de cifrado
 */
int send_file(int sock, const char *filename, char key) {
    
    // Abre el archivo en modo lectura binaria
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        perror("[-] No se pudo abrir el archivo");
        return 1;
    }

    char buffer[BUFFER_SIZE];                                           // Buffer para almacenar los datos leidos del archivo
    size_t bytes;

    while ((bytes = fread(buffer, 1, BUFFER_SIZE, fp)) > 0)             // Miestras haya datos en el archivo
    {

        // Cifra el buffer usando XOR
        xor_encrypt(buffer, bytes, key);                                // Llama a la funcion xor_encrypt para cifrar los datos

        // Envia el buffer cifrado al servidor
        send(sock, buffer, bytes, 0);                                   // Envia los datos cifrados al servidor en lotes de 1024 bytes                          
    }
    fclose(fp);
    return 0;
}


/**
 * * Funcion principal del cliente que se conecta al servidor y envia un archivo cifrado.
 */
int main() {

    // Configura el socket del cliente
    const char *ip = "127.0.0.1";
    int port = 9000;
    char key = 0x5A;
    const char *filename = "ClientFiles/el_quijote.txt";

    // Se crea el socket de cliente
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("[-] Creacion del socket fallida");
        return 1;
    }
    printf("[+] Socket creado correctamente.\n");

    // Configura direccion del servidor
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = inet_addr(ip);

    // Conecta al servidor
    if (connect(sock, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("[-] Conexion fallida");
        close(sock);
        return 1;
    }
    printf("[+] Conectado al servidor %s:%d\n", ip, port);

    // Envia clave
    send(sock, &key, sizeof(key), 0);

    // Enviar archivo
    if (send_file(sock, filename, key) == 0)
        printf("[+] Archivo enviado correctamente.\n");
        
    close(sock);
    return 0;
}

// Nota: El codigo para el main toma como referencia los siguientes enlaces:
//       https://www.youtube.com/watch?v=io2G2yW1Qk8
//       https://idiotdeveloper.com/tcp-client-server-programming-c-guide/