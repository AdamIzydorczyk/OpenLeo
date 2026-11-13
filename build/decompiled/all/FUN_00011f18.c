
undefined8 __regparm3 FUN_00011f18(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  undefined1 extraout_DL;
  
  iVar1 = FUN_00011eb8();
  if (iVar1 != 0) {
    out(DAT_000ae644 + 0xc,extraout_DL);
  }
  return CONCAT44(param_2,(uint)(iVar1 != 0));
}

