import subprocess, numpy as np, sys
W,H=640,400
cmd=['ffmpeg','-v','error','-ss','204','-t','206','-i','gp.mp4','-vf','crop=640:400:0:40','-f','rawvideo','-pix_fmt','rgb24','-']
p=subprocess.Popen(cmd,stdout=subprocess.PIPE)
out=[]; i=0
while True:
    b=p.stdout.read(W*H*3)
    if len(b)<W*H*3: break
    f=np.frombuffer(b,dtype=np.uint8).reshape(H,W,3).astype(np.int16)
    m=(f[...,0]>190)&(f[...,1]>50)&(f[...,1]<125)&(f[...,2]<40)
    m[312:,:64]=False; m[330:,560:]=False
    ys,xs=np.nonzero(m)
    if len(ys)>250:
        out.append((i,len(ys),int(xs.min()),int(xs.max()),int(ys.min()),int(ys.max()),float(xs.mean()),float(ys.mean())))
    else: out.append((i,len(ys),-1,-1,-1,-1,-1,-1))
    i+=1
np.save('track.npy',np.array(out,dtype=float)); print(i)
