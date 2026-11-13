
undefined4 __regparm3 FUN_0001f760(int param_1,int *param_2)

{
  int *piVar1;
  int iVar2;
  
  piVar1 = (int *)(**(code **)(*(int *)(param_1 + 8) + 0x68))();
  iVar2 = *param_2 - *piVar1;
  if ((iVar2 < 0x6f) || (0x13f < iVar2)) {
    return 0;
  }
  if (iVar2 < 0xb4) {
    return 3;
  }
  if (iVar2 < 0xfa) {
    return 2;
  }
  return 1;
}

