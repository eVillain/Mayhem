#ifndef CRC32_h
#define CRC32_h

#include <stddef.h>
#include <stdint.h>

/* CRC-32C (iSCSI) polynomial in reversed bit order. */
#define POLY 0x82f63b78

/* CRC-32 (Ethernet, ZIP, etc.) polynomial in reversed bit order. */
/* #define POLY 0xedb88320 */

static uint32_t crc32c(uint32_t crc, const unsigned char *buf, size_t len)
{
    int k;
    
    crc = ~crc;
    while (len--) {
        crc ^= *buf++;
        for (k = 0; k < 8; k++)
            crc = crc & 1 ? (crc >> 1) ^ POLY : crc >> 1;
    }
    return ~crc;
}

#endif /* CRC32_h */
