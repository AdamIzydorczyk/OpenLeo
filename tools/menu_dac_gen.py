#!/usr/bin/env python3
"""menu_dac_gen.py - regenerate ported/ll_menu_dac.h from a hunt13-style capture.

Inputs:
  VRAM.BIN   256 KiB xp dump of the vga.vram BAR alias (QEMU `stop` +
             `xp /65536b 0xfd000000` x4) taken at a chapter-map menu frame.
  FRAME.PNG  the matching `screendump` (640x400).
  OUT.H      header to write.

The menu framebuffer is LINEAR 8bpp 320x200 at vram+0, displayed
2x2-doubled on the 640x400 CRTC. Index bytes from VRAM x screendump
colors give the menu DAC exactly; every used index must map to a single
color or we abort (layout assumption broken).
"""
import sys

import numpy as np
from PIL import Image


def main(vram_path, png_path, out_path):
    blob = np.frombuffer(open(vram_path, 'rb').read(), np.uint8)
    art = blob[:64000].reshape(200, 320)
    g = np.asarray(Image.open(png_path).convert('RGB'), np.uint8)
    if g.shape[:2] != (400, 640):
        sys.exit('frame must be 640x400, got %r' % (g.shape,))
    cols, inv = np.unique(g.reshape(-1, 3), axis=0, return_inverse=True)
    cid = inv.reshape(400, 640)[0::2, 0::2].astype(np.int64)
    if not (g[0::2, 0::2] == g[1::2, 1::2]).all():
        sys.exit('frame is not 2x2-doubled; wrong capture state')
    M = np.bincount(art.ravel().astype(np.int64) * len(cols) + cid.ravel(),
                    minlength=256 * len(cols)).reshape(256, len(cols))
    used = np.nonzero(M.sum(axis=1))[0]
    if (M.sum(axis=1) > 0).sum() and (M > 0).sum(axis=1)[used].max() != 1:
        sys.exit('index->color map is ambiguous: wrong VRAM layout')
    pal = np.zeros((256, 3), np.uint8)
    for i in used:
        pal[i] = cols[np.nonzero(M[i])[0][0]]

    with open(out_path, 'w') as f:
        f.write(HEADER_TOP)
        for k in range(0, 256, 4):
            vals = ['0x%08Xu' % (0xFF000000 | int(pal[i][0]) << 16 |
                                 int(pal[i][1]) << 8 | int(pal[i][2]))
                    if i in set(used.tolist()) else '0xFF000000u'
                    for i in range(k, k + 4)]
            f.write('    ' + ', '.join(vals) + ',\n')
        f.write('};\n\n#endif /* LL_MENU_DAC_H */\n')
    print('%s: %d used indices' % (out_path, len(used)))


HEADER_TOP = """/*
 * ll_menu_dac.h - the chapter-map menu's actual VGA DAC palette.
 *
 * Ground truth recovered from the original engine, not a guess: the QEMU
 * guest was frozen at the menu's content frame and 256 KiB of vga.vram
 * (BAR alias @0xfd000000) was xp-dumped; the menu framebuffer is LINEAR
 * 8bpp 320x200 at vram+0 shown 2x2-doubled on the 640x400 CRTC (index
 * plane reproduces the screendump with zero color ambiguity over 209
 * used indices). Index -> DAC triples below are the screendump colors
 * of those indices (QEMU renders the DAC verbatim). Same index space as
 * the ANIMACJE/MENU .SEQ sprites: idx 15 = corner-word green, 90..95 =
 * BERLO dark golds, 218/223/224/230 = BERLO bright golds, 199/200/202 =
 * island sand, 44/46/51/102..107 = sea blues, 1 = near-black key.
 * Generated from /tmp/opencode/hunt13 capture (see PLAN 8b); regenerate
 * with tools/menu_dac_gen.py. Unused indices -> black.
 */
#ifndef LL_MENU_DAC_H
#define LL_MENU_DAC_H

static const unsigned int ll_menu_dac[256] = {
"""

if __name__ == '__main__':
    if len(sys.argv) != 4:
        sys.exit(__doc__)
    main(sys.argv[1], sys.argv[2], sys.argv[3])
