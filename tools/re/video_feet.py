import subprocess, numpy as np, sys
t0=float(sys.argv[1]); dur=float(sys.argv[2])
W,H=640,400
cmd=['ffmpeg','-v','error','-ss',str(t0),'-t',str(dur),'-i','gp.mp4','-vf','crop=640:400:0:40','-f','rawvideo','-pix_fmt','rgb24','-']
p=subprocess.Popen(cmd,stdout=subprocess.PIPE); i=0
while True:
    b=p.stdout.read(W*H*3)
    if len(b)<W*H*3: break
    f=np.frombuffer(b,dtype=np.uint8).reshape(H,W,3).astype(np.int16)
    s=(f[...,0]>=125)&(f[...,0]<=185)&(f[...,1]<15)&(f[...,2]<15); s[300:,:120]=False
    ys,xs=np.nonzero(s)
    if len(ys)<15: print('%.2f none'%(t0+i/25)); i+=1; continue
    cx,cy=int(xs.mean()),int(ys.mean())
    y0,y1=max(0,cy-10),min(H,cy+70); x0,x1=max(0,cx-45),min(W,cx+45)
    reg=f[y0:y1,x0:x1]
    # shoes: yellow (R>200,G>170,B<70) and cyan-blue soles; take lowest yellow row
    yel=(reg[...,0]>200)&(reg[...,1]>170)&(reg[...,2]<70)
    yy,xx=np.nonzero(yel)
    fy=(yy.max()+y0) if len(yy)>=6 else -1
    print('%.2f shorts_y=%5.1f feet_y(screen)=%4d  -> world %.1f   x=%.1f'%(t0+i/25,cy,fy,fy/2,cx/2))
    i+=1
