
undefined4 __regparm3 FUN_0002f3e0(int param_1,int *param_2)

{
  int *piVar1;
  undefined4 uVar2;
  
  piVar1 = (int *)(**(code **)(*(int *)(param_1 + 8) + 0x4c))();
  if ((((*param_2 < *piVar1 + -0xf) || (piVar1[2] + 0xf < *param_2)) ||
      (param_2[1] < piVar1[1] + -5)) || (piVar1[3] + 5 < param_2[1])) {
    uVar2 = 0;
  }
  else {
    uVar2 = 1;
  }
  return uVar2;
}

