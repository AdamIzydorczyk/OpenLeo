
void __regparm2 FUN_00051660(undefined4 *param_1,undefined4 param_2,int param_3)

{
  undefined4 uVar1;
  undefined4 *puVar2;
  int extraout_ECX;
  undefined4 unaff_EBX;
  
  FUN_00049920(param_2,&DAT_00092115,param_1 + 4);
  param_1[4] = (param_3 * param_1[4]) / 1000;
  if (extraout_ECX < 0) {
    param_1[5] = 1;
    param_1[2] = unaff_EBX;
    param_1[3] = extraout_ECX + 1;
    param_1[3] = -(extraout_ECX + 1);
  }
  else if (extraout_ECX == 0) {
    param_1[3] = 0;
    uVar1 = FUN_00044110();
    param_1[2] = uVar1;
  }
  else {
    param_1[5] = 0;
    param_1[2] = unaff_EBX;
    param_1[3] = extraout_ECX + -1;
  }
  while (puVar2 = (undefined4 *)FUN_00045df0(), puVar2 != (undefined4 *)0x0) {
    *puVar2 = *param_1;
    *param_1 = puVar2;
  }
  return;
}

