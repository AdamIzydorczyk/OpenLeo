#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "llrs.h"
int main(int argc,char**argv){llrs c; if(llrs_open(&c,argv[1]))return 1;
 const llrs_slot*b=llrs_find(&c,argv[2]); if(!b)return 2;
 llrs_slot*r;unsigned n; llrs_block(&c,b,&r,&n);
 for(unsigned i=0;i<n;i++) if(!strcmp(r[i].name,argv[3])){unsigned char*o;long len=llrs_read(&c,&r[i],&o); fwrite(o,1,len,stdout);return 0;}
 return 3;}
