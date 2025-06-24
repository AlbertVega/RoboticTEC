#ifndef UTILS_H
#define UTILS_H

#include <limits.h>

#define MAX_WORDS 10000
#define MAX_WORD_LEN 128

typedef struct {
    char word[MAX_WORD_LEN];
    int count;
} WordEntry;

void xor_decrypt(char *data, size_t len, char key);
void split_file(const char *filename);
void most_freq_word(const int *ports, int n);

#endif // UTILS_H