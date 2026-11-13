
undefined8 __regparm3 FUN_00011e10(undefined4 param_1,undefined4 param_2)

{
  byte bVar1;
  int iVar2;
  
  iVar2 = 0;
  do {
    iVar2 = iVar2 + 1;
    bVar1 = in(DAT_000ae644 + 6);
  } while (iVar2 < 200);
  return CONCAT44(param_2,(uint)bVar1);
}

