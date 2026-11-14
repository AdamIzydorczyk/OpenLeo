/* is_stream.h - LongSoft iNFOSTREAM .IS cutscene codec (v2.01), portable C.
 *
 * This is a 1:1 port of tools/is_decode.py, whose semantics were pinned
 * against a 230k-event live trace of the game codec (PLAN.md S3): chunk
 * framing, band opcode grammar, frame-level control opcodes, framebuffer
 * model (640x480 linear / 320x200 Mode X planar) and 6-bit DAC palette.
 * Keep the two in sync; python remains the reference for verification.
 */
#ifndef IS_STREAM_H
#define IS_STREAM_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* frame-level control opcodes (B & 0xB8 mode field) */
#define IS_OP_SOLID   0x00
#define IS_OP_LUT1    0x08
#define IS_OP_LUT2    0x10
#define IS_OP_LUT4    0x18
#define IS_OP_RAW     0x20
#define IS_OP_EOB     0x88
#define IS_OP_SKIP    0x90
#define IS_OP_RPT8    0x98
#define IS_OP_PAL     0xA0
#define IS_OP_RPT16   0xA8
#define IS_OP_ESC     0xB0
#define IS_OP_END     0xB8

#define IS_HDR_W      0x59   /* u16 */
#define IS_HDR_H      0x5B   /* u16 */
#define IS_HDR_FPS    0x5D   /* u8  */
#define IS_CHUNK_BASE 0x5F   /* first chunk header */

typedef struct {
    uint8_t *data;
    size_t   size;
} is_buf;

typedef struct {
    int      ok;         /* 0 = stream desync/out-of-bounds, stop */
    unsigned chunks, frames, esc, esc_bytes, palette;
    unsigned solid, lut1, lut2, lut4, raw, skip, eob, unknown;
    unsigned repeat;     /* last RPT8/RPT16 operand (player handshake) */
} is_stats;

typedef struct {
    is_buf   buf;        /* whole .IS file (owned) */
    int      w, h;       /* 320x200 (Mode X planar) or 640x480 (linear) */
    int      fps;
    int      modex;
    /* chunk table: opcode-stream payload of each `LLRS` chunk (audio
     * payloads are skipped; their bytes live inside ESC regions) */
    unsigned nchunks;
    struct chunk { size_t pos, len; uint16_t tail; } *chunks;
    /* persistent decode state */
    uint8_t *fb;         /* w*h, plane-linear for Mode X */
    uint8_t  pal[768];   /* 6-bit DAC triples */
    is_stats st;
} is_stream;

/* load/free: reads the file into memory and indexes chunk payloads. */
int  is_open(is_stream *s, const char *path);
void is_close(is_stream *s);

/* frame callback: idx is 0-based decoded-frame index. */
typedef void (*is_frame_cb)(void *user, unsigned idx,
                            const is_stream *s);

/* decode the whole stream (all chunks), invoking cb after every frame.
 * returns 0 on clean END/EOF walk, -1 on desync (partial state kept). */
int is_decode_all(is_stream *s, is_frame_cb cb, void *user,
                  unsigned max_frames);

/* one chunk payload at a time (player model: 1b1a0 feeds one chunk per
 * call; fb/pal persist in s). returns new stream index, -1 = done/desync. */
size_t is_decode_chunk(is_stream *s, size_t chunk_i, is_frame_cb cb,
                       void *user, unsigned *frames_in_out);

/* audio regions: ESC payloads carry the interleaved PCM/SFX audio
 * (tools/is_audio.py semantics). cb gets (user, ptr, len) per region.
 * range variant walks only chunks [first, first+count) - used by the
 * chunk-at-a-time player to enqueue audio in sync with video. */
typedef void (*is_esc_cb)(void *user, const uint8_t *pcm, unsigned len);
void is_walk_audio(is_stream *s, is_esc_cb cb, void *user);
void is_walk_audio_range(is_stream *s, unsigned first, unsigned count,
                         is_esc_cb cb, void *user);

/* output helpers ------------------------------------------------------- */

/* uninterleaved (display-order) view of fb: rows top-to-bottom.
 * Mode X: plane p row r -> display row 4r+p. Returns malloc'd w*h or NULL. */
/* P-12: reuse-a-frame variant (the player presents at ~25 fps and should
 * not malloc a full framebuffer per frame). out must be w*h bytes.
 * Same semantics as is_linearize, writing into the caller's buffer. */
int is_linearize_into(const is_stream *s, uint8_t *out);
uint8_t *is_linearize(const is_stream *s);

/* palette 6-bit -> 8-bit: v<<2 (live DAC verified vs QEMU screendumps;
 * occasional +/- rounding deltas are DAC artifacts, not content). */
void is_palette_rgb8(const is_stream *s, uint8_t rgb[768]);

/* write a P6 PPM of current fb+pal (display order). returns 0 ok. */
int is_write_ppm(const is_stream *s, const char *path);

#ifdef __cplusplus
}
#endif
#endif /* IS_STREAM_H */
