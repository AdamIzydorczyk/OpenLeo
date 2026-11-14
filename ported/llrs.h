/* Reader for LongSoft "llrs" resource containers (ANIMACJE, ETAPY, SOUND,
 * LEVELDES). Format verified against originals and mirrored by
 * tools/llrs_unpack.py (the reference implementation).
 *
 * Container layout (little-endian):
 *   header 0x00: magic 'llrs', u32 version(==1), u32 capacity (number of
 *            26-byte table slots), u32 zero, u32 actual (used count)
 *   table  0x14: capacity slots of 26 bytes: char key[14]; u32 id
 *            (1 = payload file, 2 = index/text block, 0 = free/padding);
 *            u32 dataOff (absolute in file, >= data region); u32 size
 *   data   0x14 + 26*capacity: payloads.
 *
 * Index blocks (slots with id==2 in ANIMACJE/ETAPY): a nested 26-byte-slot
 * table WITHOUT the llrs header: u32 count, then count records of
 * key[14] + id + dataOff + size, where dataOff is again ABSOLUTE in the
 * container file. id==1 records there hold named payloads (SEQ/PCX/PCM...);
 * id==2 records hold nested 0x1450-byte leaf blocks (sprite data); id==0
 * records are archive padding with binary keys. Keys may be binary, so
 * lookups must go through the 14-byte key, not a C string.
 */
#ifndef LLRS_H
#define LLRS_H

#include <stddef.h>
#include <stdio.h>

#define LLRS_KEYLEN 14

typedef struct {
    unsigned char key[LLRS_KEYLEN]; /* raw 14-byte key (may be binary)     */
    char          name[LLRS_KEYLEN + 1]; /* printable form, empty if binary */
    unsigned      id, off, size;
} llrs_slot;

typedef struct {
    FILE      *f;
    unsigned   cap, actual;
    size_t     data_start;
    long       fsize;     /* container length, for range-validating reads */
    llrs_slot *slots;   /* cap entries, id==0 for free ones */
    unsigned   used;    /* count of id!=0 slots             */
} llrs;

/* Opens container path. Returns 0 on success, non-zero on error/magic. */
int  llrs_open(llrs *c, const char *path);
void llrs_close(llrs *c);

/* Container-level lookup: case-insensitive printable name. NULL if absent. */
const llrs_slot *llrs_find(const llrs *c, const char *name);

/* Nested index block of slot s (id==2, printable-key payload tables only).
 * Parses the block into recs (malloc'd, nrecs entries, id==0 kept for
 * key positions). Returns 0 on success. Caller frees via llrs_block_free. */
int  llrs_block(const llrs *c, const llrs_slot *s, llrs_slot **recs,
                unsigned *nrecs);
void llrs_block_free(llrs_slot *recs);

/* Reads payload of slot s into malloc'd buffer; returns size or -1. */
long llrs_read(const llrs *c, const llrs_slot *s, unsigned char **out);

#endif /* LLRS_H */
