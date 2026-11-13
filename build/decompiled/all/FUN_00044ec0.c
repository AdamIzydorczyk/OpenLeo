
undefined4 __regparm3 FUN_00044ec0(int *param_1)

{
  int iVar1;
  int extraout_ECX;
  int iVar2;
  
  iVar2 = 0;
  if (0 < param_1[1]) {
    iVar1 = 0;
    do {
      iVar1 = (**(code **)(*(int *)(*param_1 + iVar1 + 8) + 8))();
      if (iVar1 != 0) {
        return 1;
      }
      iVar2 = iVar2 + 1;
      iVar1 = extraout_ECX + 0xc;
    } while (iVar2 < param_1[1]);
  }
  return 0;
}

