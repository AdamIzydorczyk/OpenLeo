#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ll_seq.h"
#include "ll_pcx.h"
/* sq PCX_blob SEQ_blob out.ppm : each drawable element in a 80x80 cell, anchor at cell (30,40) */
static unsigned char *slurp(const char*p,long*n){FILE*f=fopen(p,"rb");fseek(f,0,SEEK_END);*n=ftell(f);fseek(f,0,SEEK_SET);unsigned char*b=malloc(*n);fread(b,1,*n,f);fclose(f);return b;}
int main(int argc,char**argv){
 long n1,n2; unsigned char*pc=slurp(argv[1],&n1),*sb=slurp(argv[2],&n2);
 ll_pcx img; if(ll_pcx_decode(pc,n1,&img)){puts("pcx fail");return 1;}
 ll_seq s; if(ll_seq_parse(sb,n2,&s)){puts("seq fail");return 1;}
 int cols=8, cell=80; int rows=(s.count+cols-1)/cols; int W=cols*cell,H=rows*cell;
 unsigned char*cv=calloc(W*H,1);
 for(unsigned i=0;i<s.count;i++){
   unsigned char*c=calloc(cell*cell,1);
   ll_seq_draw_elem_anchor(&s,&s.els[i],c,cell,cell,30,40,0,NULL);
   for(int y=0;y<cell;y++)memcpy(cv+((i/cols)*cell+y)*W+(i%cols)*cell,c+y*cell,cell);
   free(c);
   printf("el %u: w0 %u arr_n %u b15 %d b17 %d f0 %d f2 %d\n",i,s.els[i].w0,s.els[i].arr_n,(short)s.els[i].b15,(short)s.els[i].b17,(short)s.els[i].f0,(short)s.els[i].f2);
 }
 FILE*o=fopen(argv[3],"wb");fprintf(o,"P6\n%d %d\n255\n",W,H);
 for(int i=0;i<W*H;i++){unsigned char v=cv[i];fputc(img.pal[3*v],o);fputc(img.pal[3*v+1],o);fputc(img.pal[3*v+2],o);} fclose(o);
 return 0;}
