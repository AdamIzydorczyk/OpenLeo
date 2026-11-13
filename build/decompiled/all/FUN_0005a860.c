
undefined4 * __regparm3 FUN_0005a860(undefined4 *param_1)

{
  int iVar1;
  int iVar2;
  int extraout_ECX;
  byte unaff_BL;
  
  *param_1 = &PTR_LAB_000a2ac0;
  if ((unaff_BL & 0x10) == 0) {
    if ((unaff_BL & 8) != 0) {
      if ((unaff_BL & 2) == 0) {
        iVar1 = FUN_0007607b();
        iVar2 = 0;
      }
      else {
        iVar1 = FUN_0007607b();
        iVar2 = 0;
      }
      goto LAB_0005a8de;
    }
    if ((unaff_BL & 2) != 0) {
      iVar1 = FUN_0007607b();
      iVar2 = 0;
      goto LAB_0005a8de;
    }
  }
  iVar1 = FUN_0007607b();
  iVar2 = 0;
LAB_0005a8de:
  while ((param_1[1] = iVar1, iVar1 == 0 && (iVar2 < 500))) {
    iVar1 = FUN_0007607b();
    iVar2 = extraout_ECX;
  }
  return param_1;
}

