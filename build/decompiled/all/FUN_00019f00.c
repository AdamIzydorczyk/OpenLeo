
undefined8 __regparm3 FUN_00019f00(undefined4 param_1,undefined4 param_2)

{
  byte bVar1;
  byte bVar2;
  uint uVar3;
  undefined2 uVar4;
  undefined6 uVar5;
  
  bVar1 = in(DAT_000c40cc);
  out(DAT_000c40cc,bVar1 | (byte)DAT_000c40d8);
  uVar5 = FUN_00075c5a();
  DAT_000c4100 = (undefined2)((uint6)uVar5 >> 0x20);
  DAT_000c40fc = (undefined4)uVar5;
  FUN_000193e0();
  FUN_00075c8c();
  bVar1 = in(0x21);
  bVar2 = in(0xa1);
  if (DAT_000c40bc < 8) {
    uVar3 = (uint)bVar1 & ~(1 << (DAT_000c40bc & 0x1f));
    uVar4 = 0x21;
  }
  else {
    out(0x21,bVar1 & 0xfb);
    uVar3 = ~(1 << (DAT_000c40bc - 8 & 0x1f)) & (uint)bVar2;
    uVar4 = 0xa1;
  }
  out(uVar4,(char)uVar3);
  DAT_000c40a8 = 1;
  return CONCAT44(param_2,uVar3);
}

