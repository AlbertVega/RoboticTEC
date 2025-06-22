#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "utils.h"

/**
 * Divide el archivo de texto en 3 partes iguales por palabras
 * 
 * Consiste en leer un archivo de texto, contar el número total de palabras,
 * dividirlas en 3 partes iguales y guardar cada parte en un archivo separado.
 * 
 */
void split_file(const char *filename) {
    
    FILE *fp = fopen(filename, "r");
    if (!fp) return;

    mkdir("ServerFiles/Split_files", 0777);

    // Se cuenta el numero total de palabras en el archivo
    int total_words = 0;
    char temp[256];

    // Se recorre el archivo contando las palabras con fscanf y se almacena en un buffer temp
    while (fscanf(fp, "%255s", temp) == 1) {
        total_words++;
    }
    rewind(fp);

    printf("Total de palabras en el archivo: %d\n", total_words);

    // Se calcula el numero de palabras por parte
    int part_words[3];                              // Array para almacenar el numero de palabras en cada parte
    int base = total_words / 3;                     // Numero base de palabras por parte   
    int remainder = total_words % 3;                // Resto de palabras

    /*
     Se asigna el numero de palabras a cada parte recorriendo el array part_words
     La parte 1 y 2 tendra base palabras, y la parte 3 tendra base + 1 si hay resto
     */
    for (int i = 0; i < 3; i++) {
        part_words[i] = base;
        if (i < remainder) {
            part_words[i] += 1;
        }
    }

    char ***buffers = malloc(3 * sizeof(char**));   // Array de buffers para almacenar las palabras de cada parte
    int *counts = calloc(3, sizeof(int));           // Contadores de palabras para cada parte

    /*
     Recorre los 3 buffers y asigna memoria para cada uno de ellos
     Cada buffer[i] almacenara las palabras de la parte i, con un tamaño de part
     */
    for (int i = 0; i < 3; i++) {
        buffers[i] = malloc(part_words[i] * sizeof(char*));
    }

    printf("Palabras por parte: %d, %d, %d\n", part_words[0], part_words[1], part_words[2]);
    
    // Indice de las palabras leidas
    int idx = 0;                                

    /*
     Recorre las 3 partes del archivo, leyendo las palabras y almacenandolas en los buffers correspondientes
     */
    for (int i = 0; i < 3; i++) {

        /*
         Para cada parte, lee la cantidad de palabras asignadas y las almacena en el buffer
         Si la lectura fue exitosa, lee una palabra del archivo y la almacena en el buffer
         usando fscanf para leer palabras y las almacena en temp con un tamaño maximo de 255 caracteres
         */
        for (int j = 0; j < part_words[i]; j++) {

             if (fscanf(fp, "%255s", temp) == 1) {
                
                buffers[i][j] = strdup(temp);       // Copia la palabra[j] leida al buffer[i]
                counts[i]++;                        // Incrementa el contador de palabras para la parte [i]
                idx++;                              // Incrementa el índice de palabras leídas  

            }
        }
    }
    // Cierra el archivo de entrada
    fclose(fp);

    /*
     * Crea los archivos de salida para cada parte y 
     * recorre los buffers para escribir las palabras en los archivos correspondientes
     */
    for (int i = 0; i < 3; i++) {
        
        char part_filename[256];
        snprintf(part_filename, sizeof(part_filename), "ServerFiles/Split_files/part_%d.txt", i + 1);
        
        FILE *part_fp = fopen(part_filename, "w");
        if (!part_fp) {
            continue;
        }
        
        /*
         * Recorre cada palabra almacenada en el buffer correspondiente a la parte actual (buffers[i]),
         * escribiendola en el archivo de salida. Entre cada palabra, excepto la última,
         * agrega un espacio para separar las palabras. Finalmente, libera la memoria asignada.
         */
        for (int j = 0; j < counts[i]; j++) {

            fprintf(part_fp, "%s", buffers[i][j]); // Escribe la palabra[j] palabra del buffer[i]

            // Agrega un espacio entre palabras
            if (j < counts[i] - 1) { 
                fputc(' ', part_fp);
            }

            // Libera la memoria
            free(buffers[i][j]); 
        }

        // Cierra el archivo de la parte
        fclose(part_fp);

        // Libera la memoria del buffer[i]
        free(buffers[i]);
    }

    // Libera la memoria de los buffers
    free(buffers);

    // Libera la memoria de los contadores
    free(counts);

    printf("Archivo dividido en 3 partes aproximandamente iguales.\n");

}