/*
 * (C) 2014, 2015, 2016 De Rooi Elektronische Systemen.
 */

#include <stdint.h>
#include <stdlib.h>

#define RB_BUFSIZE 256

#ifdef __cplusplus

/*
 * Fixed-size byte buffer, suitable for machines without malloc().
 */
class RingBuffer
{
    private:
        uint8_t bbuf[RB_BUFSIZE];
        size_t wi;
        size_t ri;
    public:
        size_t write(const uint8_t *srcp, size_t len);
        int getNextByte();
};

extern "C" size_t RingBuffer_write(void *untyped_self, const uint8_t *srcp, size_t len);
extern "C" int RingBuffer_getNextByte();

#else

size_t RingBuffer_write(void *untyped_self, const uint8_t *srcp, size_t len);
int RingBuffer_getNextByte(void *untyped_self);

#endif

