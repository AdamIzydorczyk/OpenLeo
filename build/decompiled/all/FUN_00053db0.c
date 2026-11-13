
void __regparm3 FUN_00053db0(int param_1,undefined4 param_2,undefined4 param_3)

{
  int iVar1;
  
  iVar1 = *(int *)(param_1 + 0x54);
  if (((iVar1 != 0) && (iVar1 != DAT_000cf0e4)) && (iVar1 != 0)) {
    (**(code **)(*(int *)(iVar1 + 8) + 0x14))(param_2,param_3);
  }
  *(int *)(param_1 + 0x54) = DAT_000cf0e4;
  return;
}

