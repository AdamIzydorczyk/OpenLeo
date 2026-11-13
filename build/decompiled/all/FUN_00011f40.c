
undefined8 __regparm3 FUN_00011f40(undefined4 param_1,undefined4 param_2)

{
  byte bVar1;
  int iVar2;
  
  iVar2 = FUN_00011ee8();
  if (iVar2 == 0) {
    return CONCAT44(param_2,0xffff);
  }
  bVar1 = in(DAT_000ae644 + 10);
  return CONCAT44(param_2,(uint)bVar1);
}

