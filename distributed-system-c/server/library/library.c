// arduino.c
#include "library.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

int arduino_write(const char *device, const char *data) {
    int fd = open(device, O_WRONLY);
    printf("Writing: %s to device: %s\n", data, device);
    if (fd < 0) {
        perror("open");
        return -1;
    }
    ssize_t written = write(fd, data, strlen(data));
    if (written < 0) {
        perror("write");
        close(fd);
        return -1;
    }
    close(fd);
    return (int)written;
}


void send_word_as_binary(const char *device, const char *word) {
    for (size_t i = 0; word[i] != '\0'; ++i) {
        unsigned char ch = word[i];
        printf("Sending character: %c (ASCII: %d), ", ch, ch);
        printf("Binary representation: ");
        for (int bit = 7; bit >= 0; --bit) {
            printf("%d", (ch >> bit) & 1);
        }
        printf("\n");

        for (int bit = 7; bit >= 0; --bit) {
            if ((ch >> bit) & 1) {
                arduino_write(device, "11111111111111");
            } else {
                arduino_write(device, "00000");
            }
            // Delay de 2 segundos entre caracteres
            struct timespec ts;
            ts.tv_sec = 1;
            ts.tv_nsec = (long)(0.4 * 1e9);
            nanosleep(&ts, NULL);
        }
        
    }
}


