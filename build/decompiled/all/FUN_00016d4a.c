
undefined8 __regparm3 FUN_00016d4a(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  undefined4 extraout_ECX;
  undefined4 extraout_ECX_00;
  undefined4 uVar2;
  uint uVar3;
  
  if (0x40000 < DAT_000a8869) {
    DAT_000a8869 = 0x20;
  }
  iVar1 = FUN_000170cc();
  DAT_000a886d = DAT_000a8869;
  uVar3 = (iVar1 * 0x400 + -0x20) - DAT_000a8869;
  FUN_00016ce1();
  FUN_00016ce1();
  FUN_00016ce1();
  uVar2 = extraout_ECX;
  if (0x40000 < uVar3) {
    FUN_00016fc1();
    if (0xc0000 < uVar3) {
      FUN_00016e82();
    }
    if (0x80000 < uVar3) {
      FUN_00016e82();
    }
    FUN_00016e82();
    FUN_00016e82();
    uVar2 = extraout_ECX_00;
  }
  return CONCAT44(param_2,uVar2);
}

