
void __regparm3 FUN_00050380(undefined4 *param_1,undefined4 param_2,undefined4 param_3)

{
  int iVar1;
  
  param_1[5] = *param_1;
  param_1[6] = param_1[1];
  FUN_00054e00(param_2,param_3);
  iVar1 = (*(int *)((int)param_1 + 0xb1) << 4) / 10;
  *(undefined4 *)((int)param_1 + 0xad) = 100;
  *(int *)((int)param_1 + 0xb1) = iVar1;
  *(int *)((int)param_1 + 0x122) = iVar1;
  return;
}

