import subprocess, numpy as np, sys
t0=float(sys.argv[1]); dur=float(sys.argv[2])
W,H=640,400
cmd=['ffmpeg','-v','error','-ss',str(t0),'-t',str(dur),'-i','gp.mp4','-vf','crop=640:400:0:40','-f','rawvideo','-pix_fmt','rgb24','-']
p=subprocess.Popen(cmd,stdout=subprocess.PIPE); i=0
prev=None
while True:
    b=p.stdout.read(W*H*3)
    if len(b)<W*H*3: break
    f=np.frombuffer(b,dtype=np.uint8).reshape(H,W,3).astype(np.int16)
    m=(f[...,0]>=125)&(f[...,0]<=185)&(f[...,1]<15)&(f[...,2]<15)
    m[300:,:120]=False
    ys,xs=np.nonzero(m)
    if len(ys)>=15:
        print('%.2f n=%3d shorts(x=%5.1f,y=%5.1f) -> world(%5.1f,%5.1f)'%(t0+i/25,len(ys),xs.mean(),ys.mean(),xs.mean()/2,ys.mean()/2))
    else: print('%.2f n=%3d none'%(t0+i/25,len(ys)))
    i+=1
