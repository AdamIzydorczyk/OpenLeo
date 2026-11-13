
undefined8 __regparm3 FUN_000196d0(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  uint uVar2;
  short sVar3;
  uint uVar4;
  undefined8 uVar5;
  
  out(DAT_000c40c4,1);
  for (sVar3 = 0; sVar3 != -1; sVar3 = sVar3 + 1) {
  }
  out(DAT_000c40c4,0);
  iVar1 = FUN_00019650();
  uVar4 = 0xffff;
  uVar2 = 0;
  if (iVar1 != 0) {
    do {
      uVar2 = uVar4 & 0xffff;
      uVar4 = uVar4 - 1;
      if (uVar2 == 0) break;
      uVar5 = FUN_000196b0();
      uVar4 = (uint)((ulonglong)uVar5 >> 0x20);
    } while (((uint)uVar5 & 0xff) != 0xaa);
    uVar2 = (uint)((uVar4 & 0xffff) != 0);
  }
  return CONCAT44(param_2,uVar2);
}

