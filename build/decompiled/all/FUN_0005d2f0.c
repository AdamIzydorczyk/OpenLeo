
undefined8 __regparm3 FUN_0005d2f0(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  undefined4 uVar2;
  
  DAT_000a94f0 = 5;
  FUN_0005dcc0();
  if (DAT_000cf464 != 0) {
    FUN_0005e180();
    FUN_0005df50();
    FUN_00060a30();
    FUN_00057a00();
    FUN_00057a00();
    iVar1 = FUN_000603b0();
    if (iVar1 != 0) {
      uVar2 = 2;
      DAT_000cf464 = DAT_000cf464 + -1;
      FUN_00060a80();
      goto LAB_0005d390;
    }
    FUN_00060a80();
  }
  uVar2 = 0;
LAB_0005d390:
  return CONCAT44(param_2,uVar2);
}

