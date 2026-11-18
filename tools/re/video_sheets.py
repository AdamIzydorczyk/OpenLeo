#!/usr/bin/env python3
"""video_sheets.py - labeled contact sheets of any capture window.

PR-131 (the ZARIA.01 footage audit). Extracts a window at a chosen
fps, tiles the frames into sheet images, and labels every tile with
the time the frame is from (the -ss offset plus the frame index over
the fps) and the frame's file index, so a reading can cite
"t=45.0 #31" and find the same frame again. Frames and sheets are
written under an output dir (default /tmp/opencode/audit); game
frames never enter git.

  ./video_sheets.py reference/video/levels/ZARIA.01.mkv \
      --t0 440 --t1 458 --fps 3 --band 1152:720:0:0 --prefix r3a_end
  R3 windows use band 1152:720:0:0; zax37 gameplay uses 640:400:0:40.
  A 60 fps source sampled at fps<10 drops frames evenly; tile labels
  are computed from the requested fps, which is what ffmpeg's fps
  filter produced, so labels are exact unless the decode drops.

Sheets print to stdout as sheet_<prefix>_NN.png paths. Tile width
defaults to 320 (the band's aspect is preserved); cols to 6.
"""
import argparse
import glob
import os
import subprocess
import sys

from PIL import Image, ImageDraw


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('file')
    ap.add_argument('--t0', type=float, required=True)
    ap.add_argument('--t1', type=float, required=True)
    ap.add_argument('--fps', type=float, default=2.0)
    ap.add_argument('--band', default=None,
                    help='crop=W:H:X:Y (default: the whole frame)')
    ap.add_argument('--prefix', required=True)
    ap.add_argument('--outdir', default='/tmp/opencode/audit')
    ap.add_argument('--cols', type=int, default=6)
    ap.add_argument('--tile', type=int, default=320)
    args = ap.parse_args()

    os.makedirs(args.outdir, exist_ok=True)
    vf = ('crop=' + args.band + ',' if args.band else '') + 'fps=%g' % args.fps
    pat = os.path.join(args.outdir, '%s_%%03d.png' % args.prefix)
    cmd = ['ffmpeg', '-v', 'error', '-ss', str(args.t0), '-to',
           str(args.t1), '-i', args.file, '-vf', vf, pat]
    if subprocess.run(cmd).returncode:
        sys.exit('ffmpeg failed')

    files = sorted(glob.glob(os.path.join(
        args.outdir, '%s_*.png' % args.prefix)))
    if not files:
        sys.exit('no frames extracted')
    probe = Image.open(files[0])
    tw = args.tile
    th = max(1, int(round(tw * probe.height / probe.width)))
    lab, rows = 15, 4
    per = args.cols * rows
    for s in range(0, len(files), per):
        chunk = files[s:s + per]
        im = Image.new('RGB', (args.cols * tw,
                               len(chunk) // args.cols * (th + lab)
                               + (th + lab if len(chunk) % args.cols
                                  else 0)), (20, 20, 20))
        d = ImageDraw.Draw(im)
        for k, f in enumerate(chunk):
            fr = Image.open(f).convert('RGB').resize((tw, th))
            x, y = (k % args.cols) * tw, (k // args.cols) * (th + lab)
            im.paste(fr, (x, y))
            t = args.t0 + (s + k) / args.fps
            d.rectangle([x, y, x + 128, y + lab], fill=(0, 0, 0))
            d.text((x + 2, y + 2), 't=%.2fs #%d' % (t, s + k + 1),
                   fill=(255, 255, 0))
        out = os.path.join(args.outdir,
                           'sheet_%s_%02d.png' % (args.prefix, s // per))
        im.save(out)
        print(out)


if __name__ == '__main__':
    main()
