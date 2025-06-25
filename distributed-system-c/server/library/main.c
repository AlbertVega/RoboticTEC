#include "library.h"

int main() {
    const char *device = "/dev/ArduinoDriver3"; // Cambia esto al dispositivo correcto
    const char *data = "ééó";

    send_word_as_binary(device, data);

    return 0;
}