
undefined8 __regparm3 FUN_000177d4(undefined1 param_1,int param_2)

{
  uint uVar1;
  int iVar2;
  
  uVar1 = *(uint *)((uint)DAT_000a8863 * 4 + 0xa8524);
  iVar2 = (((uVar1 >> 1) + param_2 * 0x200) / uVar1) * 2;
  out((short)DAT_000a887b,param_1);
  out((short)DAT_000a887f,1);
  out((short)DAT_000a8883,(short)iVar2);
  return CONCAT44(DAT_000a8883,iVar2);
}

