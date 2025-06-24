#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "word_counter.h"

// Funcion para convertir una palabra a minúsculas
void toLowerCase(char *str)
{
    for (int i = 0; str[i]; i++)
    {
        str[i] = tolower(str[i]);
    }
}

// Funcion para verificar si un carácter es válido para una palabra
int isWordChar(char c)
{
    // Letras, dígitos, caracteres extendidos, apóstrofe y guion
    return isalnum((unsigned char)c) || (unsigned char)c >= 128;
}

// Funcion para agregar una palabra a la lista o incrementar su conteo
void addWord(WordCount *words, int *wordCount, const char *word)
{
    // Buscar si la palabra ya existe
    for (int i = 0; i < *wordCount; i++)
    {
        if (strcmp(words[i].word, word) == 0)
        {
            words[i].count++;
            return;
        }
    }

    // Si no existe y hay espacio, agregarla
    if (*wordCount < MAX_WORDS)
    {
        strncpy(words[*wordCount].word, word, MAX_WORD_LENGTH - 1);
        words[*wordCount].word[MAX_WORD_LENGTH - 1] = '\0'; // Asegurar terminación nula
        words[*wordCount].count = 1;
        (*wordCount)++;
    }
    else
    {
        printf("[!] Se alcanzó el límite máximo de palabras distintas (%d)\n", MAX_WORDS);
    }
}

// Funcion principal
void words_counter(const char *filename, int current_port)
{
    // Abre el archivo en modo lectura usando utilidad
    FILE *input_file = fopen(filename, "r");
    if (!input_file)
    {
        perror("[-] No se pudo abrir el archivo para contar palabras");
        return;
    }
    WordCount words[MAX_WORDS];
    int wordCount = 0;

    // Inicializar la estructura de palabras
    memset(words, 0, sizeof(words));


    // Procesar el archivo palabra por palabra
    char currentWord[MAX_WORD_LENGTH];
    int charIndex = 0;
    char c;

    while ((c = fgetc(input_file)))
    {
        if (isWordChar(c))
        {
            // Agregar carácter a la palabra actual
            if (charIndex < MAX_WORD_LENGTH - 1)
            {
                currentWord[charIndex++] = tolower(c);
            }
        }
        else if (charIndex > 0)
        {
            // Terminar la palabra actual y procesarla
            currentWord[charIndex] = '\0';
            addWord(words, &wordCount, currentWord);
            charIndex = 0;
        }

        // Si se encuentra un salto de línea, procesar la palabra actual
        if (c == EOF)
        {
            break;
        }
    }

    // Cerrar archivo de entrada
    fclose(input_file);

    // Generar el nombre de salida correspondiente
    char output_filename[256];
    snprintf(output_filename, sizeof(output_filename), "NodeFiles/words_lists/list_%d.txt", current_port);

    FILE *output_file = fopen(output_filename, "w");
    if (!output_file) {
        perror("[-] No se pudo crear el archivo de salida de palabras");
        fclose(input_file);
        return;
    }

    for (int i = 0; i < wordCount; i++)
    {
        fprintf(output_file, "%s: %d\n", words[i].word, words[i].count);
    }

    // Cerrar archivo de salida
    fclose(output_file);

    printf("[+] Proceso completado.");

    return;
}
