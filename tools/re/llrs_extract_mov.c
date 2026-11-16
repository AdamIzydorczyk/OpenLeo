#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "llrs.h"
static void walk(const llrs *c,const llrs_slot*s,const char*path){
 llrs_slot*r=NULL;unsigned n=0; if(llrs_block(c,s,&r,&n))return;
 for(unsigned i=0;i<n;i++){ if(!r[i].name[0])continue; char p[300]; snprintf(p,sizeof p,"%s_%s",path,r[i].name);
  if(r[i].id==2) walk(c,&r[i],p);
  else { size_t l=strlen(r[i].name); if(l>4&&!strcasecmp(r[i].name+l-4,".MOV")){unsigned char*o;long len=llrs_read(c,&r[i],&o); char fn[400]; snprintf(fn,sizeof fn,"/tmp/opencode/mov/%s",p); FILE*f=fopen(fn,"wb"); fwrite(o,1,len,f); fclose(f); free(o);} } }
 llrs_block_free(r);}
int main(int argc,char**argv){llrs c; if(llrs_open(&c,argv[1]))return 1; for(unsigned i=0;i<c.cap;i++) if(c.slots[i].id==2&&c.slots[i].name[0]) walk(&c,&c.slots[i],c.slots[i].name); return 0;}
