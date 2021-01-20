/*
 * (C) 2016 Onwrikbaar <onwrikbaar@hotmail.com>
 */

#include <stdlib.h>
#include "arduino.h"
#include "JsonParser.h"


size_t JsonParser::write(const uint8_t *buf, const size_t nb)
{
    static uint8_t wbuf[100] = "# ";
    static short wbi = 2;

    // This is just test code. It echoes incoming lines back to the console with '# ' prepended.
    for (int i = 0; i < nb; i++) {
        const uint8_t b = buf[i];
        if (b != '\n' && wbi < sizeof(wbuf) - 1) {
            wbuf[wbi++] = b;
        } else {
            wbuf[wbi] = '\0';
            Serial.println((char *)wbuf);
            wbi = 2;
        }
    }
    return nb;
}

