#ifndef WORD_COUNTER_H
#define WORD_COUNTER_H

#define MAX_WORDS 10000
#define MAX_WORD_LENGTH 50

// Estructura para almacenar palabras y sus conteos
typedef struct {
    char word[MAX_WORD_LENGTH];
    int count;
} WordCount;

void words_counter(const char *filename, int puerto);

#endif