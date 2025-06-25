// arduino.h
#ifndef LIBRARY_H
#define LIBRARY_H

int arduino_write(const char *device, const char *data);
void send_word_as_binary(const char *device, const char *data);

#endif