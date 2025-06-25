#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "utils.h"
#include "../library/library.h"

/**
 * * Funcion para desencriptar datos usando XOR.
 * *
 * * @param data: Puntero a los datos a desencriptar.
 * * @param len: Longitud de los datos.
 * * @param key: Clave de un solo byte para la operación XOR.
 */
void xor_decrypt(char *data, size_t len, char key) {
    for (size_t i = 0; i < len; i++) {
        data[i] ^= key;                                         // Aplica XOR entre cada byte y la clave
    }
}


/**
 * * Funcion para dividir un archivo de texto en 3 partes iguales.
 * *
 * * @param filename: Nombre del archivo a dividir.
 */
void split_file(const char *filename) {

    FILE *fp = fopen(filename, "r");                            // Abre el archivo en modo lectura
    if (!fp) return;

    int number_words = 0;                                       // Contador de palabras
    char temp[256];                                             // Buffer temporal para almacenar palabras

    while (fscanf(fp, "%255s", temp) == 1)                      // Lee el numero total de palabras en el archivo
    {
        number_words++;
    }
    rewind(fp);

    int part_words[3];                                          // Arreglo para almacenar el numero de palabras en cada parte
    int base = number_words / 3;                                // Toma la parte entera de la division del numero total de palabras entre 3
    int remainder = number_words % 3;                           // Calcula el resto de la division para distribuir las palabras restantes


    for (int i = 0; i < 3; i++)                                 // Asigna el numero de palabras a cada parte, considerando el resto
    {
        part_words[i] = base;               
        if (i < remainder)          
        {
            part_words[i] += 1;
        }
    }

    char ***buffers = malloc(3 * sizeof(char **));              // Asigna memoria para los buffers de palabras y para los contadores de cada parte
    int *counts = calloc(3, sizeof(int));           

    for (int i = 0; i < 3; i++)                                 // Asigna memoria para cada buffer de palabras
    {
        buffers[i] = malloc(part_words[i] * sizeof(char *));
    }

    int idx = 0;  


    // Por cada parte, lee las palabras del archivo y las almacena en buffers si

    for (int i = 0; i < 3; i++)                                 // recorre cada parte del archivo
    {
        for (int j = 0; j < part_words[i]; j++)         // recorre el numero de palabras asignadas a esa parte
        {
            if (fscanf(fp, "%255s", temp) == 1)         // si se lee una palabra correctamente del archivo
            {
                buffers[i][j] = strdup(temp);           // se duplica la palabra en el buffer correspondiente
                counts[i]++;
                idx++;
            }
        }
    }
    fclose(fp);

    
    // Por cada parte, crea un archivo y escribe las palabras almacenadas en el buffer  

    for (int i = 0; i < 3; i++)                                 // recorre cada parte del archivo                                   
    {
        char part_filename[256];
        snprintf(part_filename, sizeof(part_filename), "ServerFiles/Split_files/part_%d.txt", i + 1);

        FILE *part_fp = fopen(part_filename, "w");

        for (int j = 0; j < counts[i]; j++)             // recorre cada palabra almacenada en el buffer de esa parte
        {
            fprintf(part_fp, "%s", buffers[i][j]);      // escribe la palabra en el archivo correspondiente
            if (j < counts[i] - 1) fputc(' ', part_fp); 
            free(buffers[i][j]);
        }
        fclose(part_fp);
        free(buffers[i]);
    }
    free(buffers);
    free(counts);
}


/**
 * * Funcion para mostrar la palabra con mas ocurrencias en los archivos de listas.
 * *
 * * @param ports: Arreglo de puertos de los servidores.
 * * @param nodes: Numero de servidores.
 */
void most_freq_word(const int *ports, int nodes) {
    
    WordEntry words[MAX_WORDS];
    int word_count = 0;

    for (int i = 0; i < nodes; i++)                             // Recorre la lista de nodos de los servidores para dar nombre a los archivos de listas
    {
        char filename[256];
        snprintf(filename, sizeof(filename), "ServerFiles/Words_lists/list_%d.txt", ports[i]);
        FILE *fp = fopen(filename, "r");
        if (!fp) continue;

        char line[256], word[MAX_WORD_LEN];                     // Buffer para almacenar cada linea y palabra
        int count;                                              // Contador de ocurrencias de la palabra

        while (fgets(line, sizeof(line), fp))                   // Lee cada linea del archivo correspondiente a la lista de palabras
        {
            if (sscanf(line, "%127[^:]:%d", word, &count) == 2) // Valida que la linea tenga el formato correcto "palabra:conteo" para extraer la palabra y su conteo
            {
                int found = 0;                                  // Bandera para indicar si la palabra ya fue encontrada

                for (int j = 0; j < word_count; j++)            // Recorre las palabras ya almacenadas
                {
                    if (strcmp(words[j].word, word) == 0)       // Compara la palabra actual con las ya almacenadas, si son iguales suma el conteo 
                    {
                        words[j].count += count;                // Suma el conteo de ocurrencias
                        found = 1;                              // Marca que la palabra ya fue encontrada  
                        break;
                    }
                }

                if (!found && word_count < MAX_WORDS)           // Si la palabra no fue encontrada y hay espacio en el arreglo de palabras
                {
                    strncpy(words[word_count].word, word, MAX_WORD_LEN-1);  // Copia la palabra al arreglo de palabras
                    words[word_count].word[MAX_WORD_LEN-1] = '\0';          // Asegura que la cadena este terminada en nulo
                    words[word_count].count = count;                        // Asigna el conteo de ocurrencias  
                    word_count++;                                           // Incrementa el contador de palabras
                }
            }
        }
        fclose(fp);
    }

    int highest_freq = 0;                                       // Indice de la palabra con mas ocurrencias
    for (int i = 1; i < word_count; i++)                        // Recorre las palabras almacenadas para encontrar la de mayor ocurrencia     
    {
        if (words[i].count > words[highest_freq].count)         // Compara el conteo de ocurrencias de la palabra actual con la de mayor ocurrencia
            highest_freq = i;
    }

    printf("La palabra con más ocurrencias es '%s' con %d apariciones.\n", words[highest_freq].word, words[highest_freq].count);
    
    // Enviar la palabra al arduino
    const char *device = "/dev/ArduinoDriver3"; // Cambia esto al dispositivo correcto
    const char *data = words[highest_freq].word; // La palabra con más ocurrencias

    send_word_as_binary(device, data);
    //send_word_as_binary(words[highest_freq].word, );
}
