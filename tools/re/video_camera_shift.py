import subprocess, numpy as np
W,H=640,400
cmd=['ffmpeg','-v','error','-ss','204','-t','206','-i','gp.mp4','-vf','crop=640:400:0:40,scale=320:200:flags=neighbor,format=gray','-f','rawvideo','-pix_fmt','gray','-']
p=subprocess.Popen(cmd,stdout=subprocess.PIPE)
w,h=320,200
prev=None; rows=[]; i=0
win=np.outer(np.hanning(h),np.hanning(w))
while True:
    b=p.stdout.read(w*h)
    if len(b)<w*h: break
    f=np.frombuffer(b,dtype=np.uint8).reshape(h,w).astype(np.float32)
    f=(f-f.mean())*win
    F=np.fft.fft2(f)
    if prev is not None:
        R=F*np.conj(prev); R/= (np.abs(R)+1e-9)
        c=np.fft.ifft2(R).real
        # top peaks
        flat=c.ravel(); idx=np.argsort(flat)[-1]
        pk=[]
        cc=c.copy()
        for _ in range(3):
            j=np.argmax(cc); y,x=divmod(j,w); v=cc[y,x]
            sy=y if y<h//2 else y-h; sx=x if x<w//2 else x-w
            pk.append((sx,sy,float(v)))
            cc[max(0,y-3):y+4,:][:, :]=cc[max(0,y-3):y+4,:]  # keep
            cc[(y-3)%h:(y+4)%h or None, :] = -1 if False else cc[(y-3)%h:(y+4)%h or None, :]
            # suppress neighbourhood (wrap-safe)
            for dy in range(-3,4):
                for dx in range(-3,4):
                    cc[(y+dy)%h,(x+dx)%w]=-1
        rows.append((i,)+tuple(v for q in pk for v in q))
    prev=F; i+=1
np.save('cam.npy',np.array(rows,dtype=float)); print(i)
