
bool __regparm3 FUN_00019470(byte param_1,char param_2)

{
  byte bVar1;
  byte bVar2;
  byte bVar3;
  byte bVar4;
  undefined2 uVar5;
  byte bVar6;
  
  if (param_1 < 8) {
    uVar5 = 0x21;
    bVar4 = 0x20;
  }
  else {
    uVar5 = 0xa1;
    bVar4 = 0xa0;
  }
  bVar1 = '\x01' << (param_1 & 7);
  bVar2 = in(uVar5);
  bVar6 = param_1 & 7 | 0x60;
  if (param_2 == '\0') {
    bVar4 = in(uVar5);
    out(uVar5,bVar4 | bVar1);
  }
  else {
    bVar3 = in(uVar5);
    out(uVar5,bVar3 & ~bVar1);
    out((ushort)bVar4,bVar6);
    if (7 < param_1) {
      FUN_00019470(bVar4,bVar1,param_2,bVar6);
    }
  }
  return (bVar2 & bVar1) == 0;
}

