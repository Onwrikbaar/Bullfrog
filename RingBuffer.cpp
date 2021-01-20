/*
 * (C) 2014, 2015, 2016 Onwrikbaar
 */

#include <string.h>
#include "RingBuffer.h"


size_t RingBuffer::write(const uint8_t *srcp, size_t len)
{
    int bytesFree = ri - wi - 1;
    size_t nb = 0;
    if (bytesFree >= 0) {
        if (len > bytesFree) return (size_t)0;
    } else {
        bytesFree += RB_BUFSIZE;
        if (len > bytesFree) return (size_t)0;
        if (wi + len >= RB_BUFSIZE) {
            memcpy(bbuf + wi, srcp, (nb = RB_BUFSIZE - wi));
            wi = 0;
            len -= nb;
        }
    }
    if (len != 0) {
        memcpy(bbuf + wi, srcp + nb, len);
        if ((wi += len) == RB_BUFSIZE) wi = 0; // Wrap around.
    }
    return nb + len;
}


int RingBuffer::getNextByte()
{
    if (ri == wi) {
/* Possible optimisation.
        wi = 0;
        ri = 0;
*/
        return -1;
    }
    const uint8_t b = *(bbuf + ri++);
    if (ri == RB_BUFSIZE) ri = 0;
    return (int)b;
}


// C interface to the above write() method.
size_t RingBuffer_write(void *untyped_self, const uint8_t *srcp, size_t len)
{
    RingBuffer *rb = static_cast<RingBuffer*>(untyped_self);
    return rb->write(srcp, len);
}

// C interface to the above getNextByte() method.
int RingBuffer_getNextByte(void *untyped_self)
{
    RingBuffer *rb = static_cast<RingBuffer*>(untyped_self);
    return rb->getNextByte();
}

