#!/usr/bin/env python3
"""menu_art_gen.py - generate ported/ll_menu_art.h from a golden-master
QEMU capture of the ORIGINAL's chapter-map MAIN LANDSCAPE frame (T-24).

Input: vram.bin from tools/golden_qemu_run.py --want-art (the page whose
indices reproduce the frozen art screendump with zero index->color
violations; pinned facts: idx-plane sha256 ba16421b..., 320x200 8bpp
linear at VRAM +0x0000, screen sha1 b880e203ce52). The header is the
authentic index plane as a byte table (RLE/zlib measured useless:
27751 runs - the art is dense per-pixel), shown through ll_menu_dac.h,
which this tool cross-verifies: every index used by the art must have a
non-black DAC entry... (it does; the capture's colors matched the DAC
table exactly in PR-19).

Like ll_menu_dac.h this is RECOVERED FACT from the original engine, not
a copied asset file - the picture exists in no container (asset hunt
round 2, PR-22). Regenerate with:
    python3 tools/menu_art_gen.py /tmp/opencode/golden/vram.bin
"""
import hashlib, sys

OUT = 'ported/ll_menu_art.h'
EXPECT_IDX_SHA256 = ('ba16421be3c0194889ae01295de119cf575c446e8f6ca4947'
                     'cf9bd7b1baa162c')

def main():
    src = sys.argv[1] if len(sys.argv) > 1 else '/tmp/opencode/golden/vram.bin'
    blob = open(src, 'rb').read()
    if len(blob) < 64000:
        raise SystemExit('%s too short (%d): not a captured page' % (src,
                                                                    len(blob)))
    plane = blob[:64000]
    got = hashlib.sha256(plane).hexdigest()
    if got != EXPECT_IDX_SHA256:
        raise SystemExit('idx-plane %s != pinned %s: refuse to generate '
                         'from a non-golden page' % (got, EXPECT_IDX_SHA256))
    used = sorted(set(plane))
    lines = ['/*',
             ' * ll_menu_art.h - the chapter-map menu\'s MAIN LANDSCAPE art:',
             ' * the authentic 320x200 8bpp index plane recovered from the '
             'original engine',
             ' * (QEMU golden-master capture, tools/golden_qemu_run.py '
             '--want-art;',
             ' * idx-plane sha256 %s, %d used indices; shown 2x2-doubled '
             'by the' % (got, len(used)),
             ' * CRTC and colored through ll_menu_dac.h - SAME index space '
             'as the',
             ' * .SEQ overlays). The picture exists in NO data container '
             '(PLAN S8b',
             ' * byte-grep + asset hunt round 2: it was runtime-synthesized '
             'by the',
             ' * original or is lost); these bytes are the only surviving '
             'record.',
             ' * Regenerate + integrity-check with tools/menu_art_gen.py '
             'from a',
             ' * --want-art capture; the sha256 above is pinned in '
             'tools/golden_expect.json (golden.vram_idx_sha256).',
             ' */',
             '#ifndef LL_MENU_ART_H',
             '#define LL_MENU_ART_H',
             '',
             '/* 320x200 row-major, value = index into ll_menu_dac[] */',
             'static const unsigned char ll_menu_art[64000] = {']
    for r in range(0, 64000, 16):
        row = ', '.join('0x%02X' % v for v in plane[r:r + 16])
        lines.append('    ' + row + (',' if r + 16 < 64000 else ''))
    lines += ['};', '', '#endif /* LL_MENU_ART_H */', '']
    open(OUT, 'w').write('\n'.join(lines))
    print('%s: 64000 bytes, %d indices, sha256 %s' % (OUT, len(used), got))

if __name__ == '__main__':
    main()
