#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "llrs.h"
int main(int argc,char**argv){llrs c; if(llrs_open(&c,argv[1]))return 1;
 const llrs_slot*b=llrs_find(&c,argv[2]); if(!b)return 2;
 llrs_slot*r;unsigned n; llrs_block(&c,b,&r,&n);
 for(unsigned i=0;i<n;i++) if(!strcmp(r[i].name,argv[3])){ llrs_slot*r2;unsigned n2; if(r[i].id==2){llrs_block(&c,&r[i],&r2,&n2); for(unsigned j=0;j<n2;j++) if(!strcmp(r2[j].name,argv[4])){unsigned char*o;long len=llrs_read(&c,&r2[j],&o); fwrite(o,1,len,stdout);return 0;}}}
 return 3;}
