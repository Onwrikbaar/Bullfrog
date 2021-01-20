/*
 * (C) 2016 Onwrikbaar <onwrikbaar@hotmail.com>
 */

#include <stdint.h>


class JsonParser
{
  private:
    uint8_t state;
    int8_t keyIndex;                            // Index of the last known key seen on input.

  public:
    size_t write(const uint8_t *buf, const size_t nb);
};

