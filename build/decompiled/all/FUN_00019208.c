
undefined6 __regparm3 FUN_00019208(undefined4 param_1,undefined2 param_2)

{
  byte bVar1;
  undefined1 uVar2;
  uint uVar3;
  
  out(DAT_000c4072,0xff);
  bVar1 = in(DAT_000c4074);
  uVar2 = in(DAT_000c4074);
  uVar3 = (uint)(ushort)(CONCAT11(bVar1,uVar2) << 8 | (ushort)bVar1);
  if (3 < DAT_000c4070) {
    uVar3 = uVar3 << 1;
  }
  return CONCAT24(param_2,uVar3);
}

