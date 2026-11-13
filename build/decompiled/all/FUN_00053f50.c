
undefined4 * __regparm3
FUN_00053f50(undefined4 *param_1,undefined4 *param_2,undefined4 param_3,int *param_4)

{
  int iVar1;
  undefined4 *puVar2;
  
  *param_1 = 0;
  param_1[1] = 2;
  param_1[2] = param_3;
  *(undefined1 *)(param_1 + 7) = *(undefined1 *)(param_2 + 3);
  puVar2 = param_1 + 4;
  for (iVar1 = 3; iVar1 != 0; iVar1 = iVar1 + -1) {
    *puVar2 = *param_2;
    param_2 = param_2 + 1;
    puVar2 = puVar2 + 1;
  }
  for (iVar1 = 0; iVar1 != 0; iVar1 = iVar1 + -1) {
    *(undefined1 *)puVar2 = *(undefined1 *)param_2;
    param_2 = (undefined4 *)((int)param_2 + 1);
    puVar2 = (undefined4 *)((int)puVar2 + 1);
  }
  if (*param_4 == 0) {
    *param_4 = (int)param_1;
  }
  else {
    FUN_00053fb0();
  }
  return param_1;
}

