
undefined4 __regparm3 FUN_00041930(int param_1,undefined4 param_2)

{
  int iVar1;
  int extraout_ECX;
  int extraout_ECX_00;
  int iVar2;
  int *unaff_EBX;
  
  iVar1 = (**(code **)(*(int *)(param_1 + 8) + 0x70))();
  iVar2 = extraout_ECX;
  if ((float)iVar1 * *(float *)(extraout_ECX + 0x140) < 0.0) {
    FUN_00053670();
    *unaff_EBX = -*unaff_EBX;
    iVar2 = extraout_ECX_00;
  }
  unaff_EBX[2] = (uint)(*(int *)(iVar2 + 0x5c) != 4);
  (**(code **)(*(int *)(iVar2 + 8) + 0xf0))();
  return param_2;
}

