
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

longlong __regparm3 FUN_00013440(undefined4 param_1,uint param_2)

{
  int iVar1;
  int iVar2;
  undefined4 uVar3;
  int extraout_ECX;
  int *extraout_ECX_00;
  int *piVar4;
  int extraout_ECX_01;
  int extraout_ECX_02;
  int extraout_ECX_03;
  int extraout_ECX_04;
  
  iVar1 = DAT_000aefe0;
  while (piVar4 = (int *)0x0, iVar1 != 0) {
    FUN_000169b4();
    iVar1 = (**(code **)(extraout_ECX + 0x10))();
    piVar4 = extraout_ECX_00;
    if (iVar1 != 0) break;
    iVar1 = *extraout_ECX_00;
  }
  if (piVar4 == (int *)0x0) {
    _DAT_000c3fb0 = s_Unknown_module_format__000902b8;
    return (ulonglong)param_2 << 0x20;
  }
  iVar1 = FUN_00015610();
  uVar3 = 0;
  if (iVar1 != 0) {
    iVar2 = (**(code **)(extraout_ECX_01 + 0xc))();
    iVar1 = extraout_ECX_02;
    uVar3 = 0;
    if (iVar2 != 0) {
      FUN_000169b4();
      uVar3 = (**(code **)(extraout_ECX_03 + 0x14))();
      iVar1 = extraout_ECX_04;
    }
    (**(code **)(iVar1 + 0x18))();
    FUN_00015638();
  }
  return CONCAT44(param_2,uVar3);
}

