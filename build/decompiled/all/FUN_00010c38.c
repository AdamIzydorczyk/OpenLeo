
undefined4 FUN_00010c38(void)

{
  int iVar1;
  uint extraout_ECX;
  undefined4 extraout_ECX_00;
  undefined8 uVar2;
  
  iVar1 = FUN_00074c13();
  if (iVar1 != 0) {
    uVar2 = FUN_00074cc8(iVar1,s__hx__hd__hd__hd__hd_0009005d,&DAT_000ade94,&DAT_000ade9a,
                         &DAT_000ade9c,&DAT_000ade9e,&DAT_000adea0);
    if ((int)uVar2 == 5) {
      FUN_000105dc((int)((ulonglong)uVar2 >> 0x20),extraout_ECX_00);
      FUN_00010690();
      FUN_00010690();
      FUN_000105dc();
      FUN_00010690();
      FUN_00010690();
      FUN_00010714();
      FUN_00010714();
      FUN_00010754();
      FUN_00010714();
      FUN_00010754();
      FUN_00010714();
      FUN_00010754();
      FUN_00010754();
      if (((extraout_ECX & 0xff) == 0xaa) && ((extraout_ECX >> 8 & 0xff) == 0x55)) {
        return 1;
      }
      return 0;
    }
  }
  return 0;
}

