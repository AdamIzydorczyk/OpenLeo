
undefined4 __regparm3 FUN_000510c0(int *param_1,undefined4 param_2,undefined4 param_3)

{
  int iVar1;
  undefined8 uVar2;
  
  FUN_00076e98(param_3);
  uVar2 = FUN_00076e98();
  if ((int)((ulonglong)uVar2 >> 0x20) != (int)uVar2) {
    param_1[2] = 0;
    return 0;
  }
  iVar1 = param_1[2];
  param_1[2] = iVar1 + 1;
  if (*(char *)(*param_1 + (iVar1 + 1) * 2) == '\0') {
    (**(code **)(param_1[1] + 4))();
    param_1[2] = 0;
  }
  return 1;
}

