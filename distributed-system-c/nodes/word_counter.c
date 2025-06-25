#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "word_counter.h"


/**
 * * Funcion para convertir una cadena a minúsculas.
 */
void toLowerCase(char *str)
{
    for (int i = 0; str[i]; i++)
    {
        str[i] = tolower(str[i]);
    }
}


/**
 * * Funcion para verificar si un carácter es parte de una palabra.
 * 
 * * Considera letras, dígitos, caracteres extendidos, apóstrofe y guion.
 */
int isWordChar(char c)
{
    // Letras, dígitos, caracteres extendidos, apóstrofe y guion
    return isalnum((unsigned char)c) || (unsigned char)c >= 128;
}


/**
 * * Funcion para agregar una palabra al contador de palabras.
 */
void addWord(WordCount *words, int *wordCount, const char *word)
{
    
    for (int i = 0; i < *wordCount; i++)                            // Para cada palabra ya registrada
    {
        if (strcmp(words[i].word, word) == 0)                       // Si la palabra ya existe
        {
            words[i].count++;                                       // Incrementar el contador 
            return;
        }
    }

    
    if (*wordCount < MAX_WORDS)                                     // Si hay espacio para una nueva palabra
    {
        strncpy(words[*wordCount].word, word, MAX_WORD_LENGTH - 1); // Copiar la palabra
        words[*wordCount].word[MAX_WORD_LENGTH - 1] = '\0';         // Asegurar terminación nula
        words[*wordCount].count = 1;                                // Inicializar el contador a 1
        (*wordCount)++;                                             // Incrementar el contador de palabras 
    }
    else                                                        
    {
        printf("[!] Se alcanzó el límite máximo de palabras distintas (%d)\n", MAX_WORDS);
    }
}


/**
 * * Funcion para contar palabras en un archivo y generar un archivo de salida.
 * 
 * * Recibe el nombre del archivo de entrada y el puerto actual.
 */
void words_counter(const char *filename, int current_port)
{
    
    FILE *input_file = fopen(filename, "r");                        // Abrir el archivo de entrada

    WordCount words[MAX_WORDS];                                     // Estructura para almacenar palabras y sus conteos
    int wordCount = 0;
    memset(words, 0, sizeof(words));                                                    


    // Procesar el archivo palabra por palabra
    char currentWord[MAX_WORD_LENGTH];
    int charIndex = 0;
    char c;

    while ((c = fgetc(input_file)) != EOF)                          // Mientras haya caracteres en el archivo
    {
        // Caso cuando el caracter es valido en si
        if (isWordChar(c))                                          // Si el carácter es parte de una palabra y es válido               
        {
            if (charIndex < MAX_WORD_LENGTH - 1)                    // Si hay espacio en la palabra actual
            {
                currentWord[charIndex++] = tolower(c);              // Convertir a minúsculas y agregar el carácter    
            }
        }

        // Caso cuando el caracter no es uno valido
        else if (charIndex > 0)                                     // Si se encuentra un carácter no válido y hay una palabra en construcción                  
        {
            currentWord[charIndex] = '\0';                          // Terminar la palabra actual    
            addWord(words, &wordCount, currentWord);                // Agregar la palabra al contador de palabras
            charIndex = 0;                                          // Reiniciar el índice de caracteres para la siguiente palabra
        }

    }

    fclose(input_file);     

    char output_filename[256];
    snprintf(output_filename, sizeof(output_filename), "NodeFiles/words_lists/list_%d.txt", current_port);

    FILE *output_file = fopen(output_filename, "w");

    for (int i = 0; i < wordCount; i++)                                     // Escribir las palabras y sus conteos en el archivo de salida  
    {
        fprintf(output_file, "%s: %d\n", words[i].word, words[i].count);    // Escribir la palabra y su conteo en el archivo de salida en el formato "palabra: conteo"
    }

    fclose(output_file);

    printf("[+] Proceso completado.");

    return;
}
