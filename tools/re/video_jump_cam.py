import subprocess, numpy as np, sys
src=sys.argv[1]; t0=float(sys.argv[2]); dur=float(sys.argv[3])
w,h=320,200
cmd=['ffmpeg','-v','error','-ss',str(t0),'-t',str(dur),'-i',src,'-vf','crop=640:400:0:40','-f','rawvideo','-pix_fmt','rgb24','-']
p=subprocess.Popen(cmd,stdout=subprocess.PIPE)
win=np.outer(np.hanning(h),np.hanning(w))
prevF=None; camx=camy=0.0; i=0
def peaks(c,n=4):
    cc=c.copy(); out=[]
    for _ in range(n):
        j=np.argmax(cc); y,x=divmod(j,w); v=cc[y,x]
        out.append((x if x<w//2 else x-w, y if y<h//2 else y-h, float(v)))
        for dy in range(-3,4):
            for dx in range(-3,4): cc[(y+dy)%h,(x+dx)%w]=-1
    return out
print('t cam(x,y) lion feet world, shorts world')
while True:
    b=p.stdout.read(640*400*3)
    if len(b)<640*400*3: break
    f=np.frombuffer(b,dtype=np.uint8).reshape(400,640,3).astype(np.int16)
    g=f[::2,::2].mean(axis=2).astype(np.float32); g=(g-g.mean())*win
    F=np.fft.fft2(g)
    if prevF is not None:
        R=F*np.conj(prevF); R/=(np.abs(R)+1e-9); c=np.fft.ifft2(R).real
        pk=[q for q in peaks(c) if q[2]>0.08]
        # camera motion = the LARGEST |shift| peak (the 100% layer moves the most)
        if pk:
            q=max(pk,key=lambda q:abs(q[0])+abs(q[1])); camx+=-q[0]; camy+=-q[1]; sh=(q[0],q[1])
        else: sh=(0,0)
    else: sh=(0,0)
    prevF=F
    s=(f[...,0]>=125)&(f[...,0]<=185)&(f[...,1]<15)&(f[...,2]<15); s[300:,:120]=False; s[330:,560:]=False
    ys,xs=np.nonzero(s)
    if len(ys)>=15:
        cx,cy=xs.mean(),ys.mean()
        reg=f[max(0,int(cy)-10):min(400,int(cy)+70),max(0,int(cx)-45):min(640,int(cx)+45)]
        yel=(reg[...,0]>200)&(reg[...,1]>170)&(reg[...,2]<70); yy,xx=np.nonzero(yel)
        fy=(yy.max()+max(0,int(cy)-10))/2 if len(yy)>=6 else float('nan')
        print('%.2f cam(%6.1f,%6.1f) shift%s  scr_x %.0f feet_scr %.1f  WORLD x %.1f feet_y %.1f shorts_y %.1f'%(t0+i/25,camx,camy,sh,cx/2,fy,cx/2+camx,fy+camy,cy/2+camy))
    else: print('%.2f cam(%6.1f,%6.1f) none'%(t0+i/25,camx,camy))
    i+=1
